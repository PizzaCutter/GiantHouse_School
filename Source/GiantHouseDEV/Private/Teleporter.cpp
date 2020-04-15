// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Teleporter.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "MotionControllerComponent.h"
#include "AI/NavigationSystemBase.h"
#include "Public/BasePawn.h"
#include "Public/TeleportArea.h"
#include "Public/TeleportSnapArea.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
//#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
//#include "Particles/ParticleSystem.h"
//#include "Kismet/GameplayStatics.h"
#include "Public/TeleportationTrace.h"
#include "Components/SplineComponent.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UTeleporter::UTeleporter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	pSnapArcSpline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

void UTeleporter::BeginPlay()
{
	Super::BeginPlay();
}

void UTeleporter::Setup(ABasePawn* parentPawn)
{
	if (parentPawn != nullptr)
	{
		pParentPawn = parentPawn;
		pPlayerController = Cast<APlayerController>(pParentPawn->GetController());
		pMotionController = Cast<UMotionControllerComponent>(GetAttachParent());

		FindAllTeleportAreas();
		DisableAllTeleportAreas();

		DisableArcVisualization();
	}
}

// Called every frame
void UTeleporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsActive())
	{
		CalculateRotation();
		VisualizeArc(DeltaTime);
	}
}

void UTeleporter::SetActive(bool newState)
{
	bIsActive = newState;
	if (newState)
	{
		EnableAllTeleportAreas();
	}
	else
	{
		DisableAllTeleportAreas();
		DisableArcVisualization();
	}

}

void UTeleporter::Teleport()
{
	FPredictProjectilePathResult result;
	if (CreateArc(result))
	{
		if (TargetTeleportArea == nullptr)
			return;

		if (!TargetTeleportArea->bAbleToTeleport)
			return;

		if (pParentPawn == nullptr)
			return;

		if (pPlayerController != nullptr)
			pPlayerController->ClientSetCameraFade(true, FColor::Black, FVector2D(0.f, 1.0f), pParentPawn->TeleportFadeTime, false);

		////SPAWN TELEPORTATION START POINT
		//if (pParentPawn->pPreviousPositionParticle != nullptr)
		//{
		//	FHitResult outhit;
		//	FVector start = GetComponentTransform().GetLocation();
		//	FVector end = GetComponentTransform().GetLocation() - FVector(0.f, 0.f, 500.f);
		//	ECollisionChannel traceChannel = ECollisionChannel::ECC_WorldStatic;
		//	FCollisionQueryParams queryParams;
		//	FCollisionResponseParams responseParams;
		//	if(GetWorld()->LineTraceSingleByChannel(outhit, start, end, traceChannel, queryParams, responseParams))
		//	{
		//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pParentPawn->pPreviousPositionParticle, outhit.Location + FVector(0.f, 0.f, 1.f));
		//	}else
		//	{
		//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pParentPawn->pPreviousPositionParticle, GetComponentTransform());
		//	}
		//}

		//SPAWN TELEPORTATION TRACE
		if (pParentPawn->TeleportationTrace != nullptr)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = pParentPawn;
			ATeleportationTrace* teleportationTrace = GetWorld()->SpawnActor<ATeleportationTrace>(pParentPawn->TeleportationTrace, pParentPawn->GetActorLocation() - FVector(0.f, 0.f, 80.f), FRotator(0.f, 0.f, 0.f), spawnParams);
			teleportationTrace->SetEndPointLocation(result.HitResult.Location + FVector(0.f, 0.f, 80.f));
		}

		//TELEPORT THE PLAYER (ROTATION & LOCATION)
		pParentPawn->SetActorLocation(result.HitResult.Location);
		FRotator newCameraRotation = CalculatedRotation;
		newCameraRotation.Yaw -= FRotator(pParentPawn->pCamera->GetRelativeTransform().GetRotation()).Yaw;
		pParentPawn->GetController()->SetControlRotation(newCameraRotation - FRotator(0.f, 90.f, 0.f));

		//THIS IS FOR KEYBOARD AND MOUSE TESTING
		if (!pParentPawn->HasHMD)
		{
			pParentPawn->SetActorLocation(pParentPawn->GetActorLocation() + FVector(0.f, 0.f, 180.f));
		}

		TargetTeleportArea->bTeleportedTo = true;
		ATeleportArea* TargetTeleportArea = nullptr;
	}
}

void UTeleporter::ResetTeleportAreas()
{
	TeleportAreas.Empty();
	TeleportSnapAreas.Empty();
	FindAllTeleportAreas();
	DisableAllTeleportAreas();
}

//---------------------------------------TELEPORT AREA FUNCTIONS----------------------------------------
void UTeleporter::FindAllTeleportAreas()
{
	for (TActorIterator<ATeleportArea> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ATeleportArea *teleportArea = *ActorItr;
		if (teleportArea != nullptr)
			TeleportAreas.Add(teleportArea);
	}
	for (TActorIterator<ATeleportSnapArea> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ATeleportSnapArea *teleportSnapArea = *ActorItr;
		if (teleportSnapArea != nullptr)
			TeleportSnapAreas.Add(teleportSnapArea);
	}
}
void UTeleporter::DisableAllTeleportAreas()
{
	for (int i = 0; i < TeleportAreas.Num(); i++)
	{
		//if(TeleportAreas[i] == nullptr || !IsValid(TeleportAreas[i]))
		//{
		//	TeleportAreas.Reset();
		//	FindAllTeleportAreas();
		//	DisableAllTeleportAreas();
		//	return;

		if (!TeleportAreas.IsValidIndex(i))
			return;

		if (TeleportAreas[i] == nullptr)
		{
			TeleportAreas.RemoveAt(i);
			i--;
			continue;
		}

		if (IsValid(TeleportAreas[i]))
			TeleportAreas[i]->SetVisible(false);
	}
}
void UTeleporter::EnableAllTeleportAreas()
{
	for (int i = 0; i < TeleportAreas.Num(); i++)
	{
		//if (TeleportAreas[i] == nullptr || !IsValid(TeleportAreas[i]) || TeleportAreas[i]->GetName() == "None")
		//{
		//	TeleportAreas.Reset();
		//	FindAllTeleportAreas();
		//	EnableAllTeleportAreas();
		//	return;
		//}
		if (!TeleportAreas.IsValidIndex(i))
			return;

		if (TeleportAreas[i] == nullptr)
		{
			TeleportAreas.RemoveAt(i);
			i--;
			continue;
		}
		if (IsValid(TeleportAreas[i]))
			TeleportAreas[i]->SetVisible(true);
	}
}

//---------------------------------------ARC FUNCTIONALITY---------------------------------------
bool UTeleporter::CreateArc(FPredictProjectilePathResult& pathResults)
{
	pParentPawn->ArcParams.StartLocation = GetComponentLocation();
	pParentPawn->ArcParams.LaunchVelocity = pMotionController->GetForwardVector() * pParentPawn->TeleportArcVelocity;
	if (UGameplayStatics::PredictProjectilePath(GetWorld(), pParentPawn->ArcParams, pathResults))
	{
		if (pathResults.HitResult.Actor == nullptr)
			return false;

		ATeleportArea* castedActor = Cast<ATeleportArea>(pathResults.HitResult.Actor);

		if (castedActor == nullptr)
			return false;

		TargetTeleportArea = castedActor;

		//Find a teleport snap area
		int teleportSnapAreaIndex = FindTeleportSnapArea(pathResults.HitResult.Location);
		if (teleportSnapAreaIndex != -1)
		{
			if (!TeleportSnapAreas.IsValidIndex(teleportSnapAreaIndex))
				return false;

			TeleportSnapArea = TeleportSnapAreas[teleportSnapAreaIndex];
			if(TeleportSnapArea == nullptr)
			{	
				TeleportSnapAreas.RemoveAt(teleportSnapAreaIndex);
				return false;
			}
			if (TeleportSnapArea == nullptr || pSnapArcSpline == nullptr || pParentPawn == nullptr)
				return false;

			pathResults.HitResult.Location = TeleportSnapArea->GetActorLocation();

			pSnapArcSpline->ClearSplinePoints(false);
			pSnapArcSpline->AddSplineWorldPoint(GetComponentLocation());
			pSnapArcSpline->AddSplineWorldPoint(pathResults.HitResult.Location);
			pSnapArcSpline->SetSplinePointType(0, ESplinePointType::Curve, false);
			pSnapArcSpline->SetSplinePointType(1, ESplinePointType::Curve, true);
			pSnapArcSpline->SetTangentAtSplinePoint(0, pParentPawn->ArcParams.LaunchVelocity, ESplineCoordinateSpace::World, true);
			pSnapArcSpline->Duration = 1.f;

			TArray<FPredictProjectilePathPointData> newPointData;
			for (float i = 0.f; i < 1.f; i += (1.f / pathResults.PathData.Num()))
			{
				newPointData.Add(FPredictProjectilePathPointData(pSnapArcSpline->GetLocationAtTime(i, ESplineCoordinateSpace::World, false), FVector(), i));
			}
			pathResults.PathData.Empty();
			pathResults.PathData = newPointData;
		}
		else
		{
			TeleportSnapArea = nullptr;
		}
		return castedActor->bIsEnabled;
	}
	return false;
}

int UTeleporter::FindTeleportSnapArea(FVector location)
{
	float closestDistance = 100000000.f;
	int closestIndex = -1;

	for (int i = 0; i < TeleportSnapAreas.Num(); i++)
	{
		if (!TeleportSnapAreas.IsValidIndex(i))
			continue;

		if (TeleportSnapAreas[i] == nullptr)
		{
			TeleportSnapAreas.RemoveAt(i);
			i--;
			continue;
		}	

		if (!TeleportSnapAreas[i]->IsAreaActive())
			continue;

		FVector snapAreaLoc = TeleportSnapAreas[i]->GetActorLocation();
		float distanceToSnapArea = FVector(location - snapAreaLoc).Size();

		if (distanceToSnapArea <= SnapRadius && distanceToSnapArea <= closestDistance)
		{
			closestDistance = distanceToSnapArea;
			closestIndex = i;
		}
	}

	return closestIndex;
}

//---------------------------------------ARC VISUALIZATION----------------------------------------
void UTeleporter::VisualizeArc(float deltaTime)
{
	FPredictProjectilePathResult result;
	if (CreateArc(result))
	{
		VisualizeArcEnd(result, true);
		VisualizeArcPath(result, deltaTime, true);
	}
	else
	{
		VisualizeArcEnd(result, false);
		VisualizeArcPath(result, deltaTime, false);
	}
}
void UTeleporter::VisualizeArcEnd(FPredictProjectilePathResult result, bool valid)
{
	if (valid)
	{
		if (pParentPawn != nullptr)
		{
			if (pParentPawn->pTeleportArrow != nullptr)
			{
				pParentPawn->pTeleportArrow->SetWorldLocation(result.HitResult.Location);
				pParentPawn->pTeleportArrow->SetWorldRotation(CalculatedRotation);
				pParentPawn->pTeleportArrow->SetVisibility(true);
			}
		}
	}
	else
	{
		if (pParentPawn != nullptr)
		{
			if (pParentPawn->pTeleportArrow != nullptr && pParentPawn->pTeleportArrowDynamicMaterial != nullptr)
			{
				//LINE TRACE BETWEEN EACH TRACEPOINT TO CHECK FOR A STATIC OBJECT SO WE CAN RENDER THE TELEPORT ARROW
				for (int i = 0; i < result.PathData.Num() - 1; i++)
				{
					if (!result.PathData.IsValidIndex(i))
						return;

					FPredictProjectilePathPointData tracePoint = result.PathData[i];
					FPredictProjectilePathPointData nextTracePoint = result.PathData[i + 1];

					//FIND A LOCATION ON A STATIC MESH
					FCollisionObjectQueryParams traceParams = FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllStaticObjects);
					FCollisionQueryParams colQueryParams;
					colQueryParams.AddIgnoredActor(Cast<AActor>(pParentPawn));

					FHitResult hit;
					if (GetWorld()->LineTraceSingleByObjectType(hit, tracePoint.Location, nextTracePoint.Location, traceParams, colQueryParams))
					{
						pParentPawn->pTeleportArrow->SetWorldLocation(hit.Location);
						pParentPawn->pTeleportArrow->SetWorldRotation(CalculatedRotation);
						pParentPawn->pTeleportArrow->SetVisibility(true);
						pParentPawn->pTeleportArrowDynamicMaterial->SetScalarParameterValue("EmissiveIntensity", pParentPawn->ArcPointMinEmissive);
						return;
					}
				}
			}
		}
	}
}
void UTeleporter::VisualizeArcPath(FPredictProjectilePathResult result, float deltaTime, bool valid)
{
	int lastIndex = -1;

	//OBJECT POOLING MESHES BUT ADD MORE IF WE HAVE TO MANY TRACEPOINTS
	if (pParentPawn == nullptr)
		return;

	if (result.PathData.Num() > pParentPawn->pTeleportTracePointsMeshes.Num())
	{
		int difference = result.PathData.Num() - pParentPawn->pTeleportTracePointsMeshes.Num();
		pParentPawn->AddMeshesToArcPathObjectPool(difference);
	}

	//SET ALL THE SPHERES AT A TRACE POINT LOCATION
	for (int i = 0; i < result.PathData.Num(); i++)
	{
		if (!pParentPawn->pTeleportTracePointsMeshes.IsValidIndex(i))
			return;

		if (pParentPawn->pTeleportTracePointsMeshes[i] == nullptr)
		{
			continue;
		}

		if (!result.PathData.IsValidIndex(i))
			return;
		if (!pParentPawn->pTeleportTracePointsMeshes.IsValidIndex(i))
			return;

		FPredictProjectilePathPointData tracePoint = result.PathData[i];
		pParentPawn->pTeleportTracePointsMeshes[i]->SetVisibility(true);
		pParentPawn->pTeleportTracePointsMeshes[i]->SetWorldLocation(tracePoint.Location);
		pParentPawn->pTeleportTracePointsMaterials[i]->SetScalarParameterValue("EmissiveIntensity", pParentPawn->ArcPointMinEmissive);
		lastIndex = i;
	}

	for (int i = lastIndex; i < result.PathData.Num(); i++)
	{
		if (!pParentPawn->pTeleportTracePointsMeshes.IsValidIndex(i))
			return;

		if (pParentPawn->pTeleportTracePointsMeshes[i] == nullptr)
		{
			continue;
		}
		pParentPawn->pTeleportTracePointsMeshes[i]->SetVisibility(false);
	}

	//BLINKING MATERIAL ANIMATION
	BlinkTimer += deltaTime;
	CurrentPoint %= pParentPawn->BlinkSteps;

	for (int i = 0; i < result.PathData.Num(); i += pParentPawn->BlinkSteps)
	{
		//ARRAY BOUND CHECKING
		if ((CurrentPoint + i) >= pParentPawn->pTeleportTracePointsMaterials.Num())
			continue;

		float emissiveValue = 0.f;
		//LOW TO HIGH EMISSIVE
		if (BlinkTimer < (pParentPawn->BlinkTime / 2.f))
		{
			float alpha = BlinkTimer / (pParentPawn->BlinkTime / 2.f);
			emissiveValue = FMath::Lerp(pParentPawn->ArcPointMinEmissive, pParentPawn->ArcPointMaxEmissive, alpha);

		}
		//HIGH TO LOW EMISSIVE
		else
		{
			float alpha = (BlinkTimer - (pParentPawn->BlinkTime / 2.f)) / (pParentPawn->BlinkTime / 2.f);
			emissiveValue = FMath::Lerp(pParentPawn->ArcPointMaxEmissive, pParentPawn->ArcPointMinEmissive, alpha);
		}

		if (!pParentPawn->pTeleportTracePointsMaterials.IsValidIndex(CurrentPoint + i))
			return;

		if(pParentPawn->pTeleportTracePointsMaterials[CurrentPoint + i] != nullptr)
			pParentPawn->pTeleportTracePointsMaterials[CurrentPoint + i]->SetScalarParameterValue("EmissiveIntensity", emissiveValue);
	}

	if (TargetTeleportArea != nullptr)
	{
		if (TargetTeleportArea->bAbleToTeleport)
		{
			if(pParentPawn->pTeleportArrowDynamicMaterial != nullptr)
				pParentPawn->pTeleportArrowDynamicMaterial->SetScalarParameterValue("EmissiveIntensity", pParentPawn->ArcPointMaxEmissive);
		}
		else
		{
			if (pParentPawn->pTeleportArrowDynamicMaterial != nullptr)
				pParentPawn->pTeleportArrowDynamicMaterial->SetScalarParameterValue("EmissiveIntensity", pParentPawn->ArcPointMinEmissive);
		}
	}

	//UPDATE BLINK TRACE POINT TO ANIMATE IT
	if (BlinkTimer >= pParentPawn->BlinkTime)
	{
		CurrentPoint++;
		BlinkTimer = 0.f;
	}


	if (valid == false)
	{
		for (int i = 0; i < pParentPawn->pTeleportTracePointsMaterials.Num(); i++)
		{
			if (!pParentPawn->pTeleportTracePointsMaterials.IsValidIndex(i))
				return;

			if(pParentPawn->pTeleportTracePointsMaterials[i] != nullptr)
				pParentPawn->pTeleportTracePointsMaterials[i]->SetScalarParameterValue("EmissiveIntensity", pParentPawn->ArcPointMinEmissive);
		}
		if (pParentPawn->pTeleportArrowDynamicMaterial != nullptr)
		pParentPawn->pTeleportArrowDynamicMaterial->SetScalarParameterValue("EmissiveIntensity", pParentPawn->ArcPointMinEmissive);
		//pParentPawn->pTeleportArrow->SetVisibility(false);
	}
}


void UTeleporter::DisableArcVisualization()
{
	for (int i = 0; i < pParentPawn->pTeleportTracePointsMeshes.Num(); i++)
	{
		if (!pParentPawn->pTeleportTracePointsMeshes.IsValidIndex(i))
			return;

		if (pParentPawn->pTeleportTracePointsMeshes[i] == nullptr)
		{
			continue;
		}
		pParentPawn->pTeleportTracePointsMeshes[i]->SetVisibility(false);
		if (pParentPawn->pTeleportArrow == nullptr)
		{
			continue;
		}
		pParentPawn->pTeleportArrow->SetVisibility(false);
	}
}

//-----------------------------------------HELP FUNCTIONS--------------------------------------------
void UTeleporter::CalculateRotation()
{
	if (TeleportSnapArea == nullptr)
	{
		CalculatedRotation = UKismetMathLibrary::FindLookAtRotation(FVector(0.f, 0.f, 0.f), AxisValue);
		CalculatedRotation += GetComponentRotation(); // +FRotator(pParentPawn->pCamera->GetRelativeTransform().GetRotation());
	}
	else
	{
		CalculatedRotation = TeleportSnapArea->GetTransform().Rotator();
		//CalculatedRotation += GetComponentRotation(); // +FRotator(pParentPawn->pCamera->GetRelativeTransform().GetRotation());
	}

	CalculatedRotation.Pitch = 0.f;
	CalculatedRotation.Roll = 0.f;
}
