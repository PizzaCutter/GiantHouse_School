// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/WorldInteractor.h"
#include "MotionControllerComponent.h"
#include "Public/IWorldInteractions.h"
#include "Engine/Engine.h"
#include "Public/BasePawn.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UWorldInteractor::UWorldInteractor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWorldInteractor::BeginPlay()
{
	Super::BeginPlay();
	UMotionControllerComponent* motionController = Cast<UMotionControllerComponent>(GetAttachParent());
	if(motionController != nullptr)
	{
		if (motionController->MotionSource == "Left")
		{
			eHAND = EHand::E_Left;
		}
		else if (motionController->MotionSource == "Right")
		{
			eHAND = EHand::E_Right;
		}
	}

	pParentPawn = Cast<ABasePawn>(GetOwner());
}


// Called every frame
void UWorldInteractor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive())
		return;

	if (!bHoveredActorLocked)
	{
		bForceGrabbedActors = false;
		TArray<AActor*> overlappingActors;
		pOverlapShape->GetOverlappingActors(overlappingActors);

		for (int i = 0; i < overlappingActors.Num(); i++)
		{
			AActor* tempActor = overlappingActors[i];
			if (tempActor == nullptr) {
				overlappingActors.RemoveAt(i);
				--i;
				continue;
			}
			if (!tempActor->GetClass()->ImplementsInterface(UIWorldInteractions::StaticClass()))
			{
				overlappingActors.RemoveAt(i);
				--i;
				continue;
			}
		}

		if (overlappingActors.Num() <= 0)
		{

			if (pParentPawn != nullptr)
			{
				ForceGrabActor(overlappingActors);
			}
			else
			{
				pParentPawn = Cast<ABasePawn>(GetOwner());
			}
		}

		AActor* tempActor = nullptr;
		USceneComponent* tempSceneComp = nullptr;
		tempActor = FindActorToHover(overlappingActors, nullptr, tempSceneComp);

		if (tempActor != pHoveredActor)
		{
			if (pHoveredActor != nullptr && pHoveredComponent != nullptr)
			{
				IIWorldInteractions::Execute_OnUnhover(pHoveredActor, this);
				IIWorldInteractions::Execute_OnHoveredComponentChanged(pHoveredActor, this, pHoveredComponent, nullptr);
				//IIWorldInteractions::Execute_OnPickupReleased(pHoveredActor, this);
				//DragStop();
			}
			//TO-DO: add world interactor DRAG STOP
			pHoveredActor = tempActor;
			if (pHoveredActor != nullptr && tempSceneComp != nullptr)
			{
				IIWorldInteractions::Execute_OnHover(pHoveredActor, this);
				IIWorldInteractions::Execute_OnHoveredComponentChanged(pHoveredActor, this, nullptr, tempSceneComp);
				//IIWorldInteractions::Execute_OnDragSp(pHoveredActor, this);
			}
		}
		else
		{
			if (tempSceneComp != pHoveredComponent && pHoveredActor != nullptr && pHoveredComponent != nullptr && tempSceneComp != nullptr)
			{
				IIWorldInteractions::Execute_OnHoveredComponentChanged(pHoveredActor, this, pHoveredComponent, tempSceneComp);
				pHoveredComponent = tempSceneComp;
			}
		}
	}

	if (bDragging)
	{
		if (pHoveredActor != nullptr)
			IIWorldInteractions::Execute_OnDrag(pHoveredActor, this);
	}
}

AActor* UWorldInteractor::FindActorToHover(TArray<AActor*> actors, AActor* foundActor, USceneComponent* foundComponent)
{
	int highestPriority = -1;
	float closestDistance = 10000000000.f;
	AActor* closestActor = nullptr;
	USceneComponent* closestComponent = nullptr;

	for (int i = 0; i < actors.Num(); i++)
	{
		AActor* tempActor = actors[i];
		if (tempActor == nullptr)
			continue;
		if (!tempActor->GetClass()->ImplementsInterface(UIWorldInteractions::StaticClass()))
			continue;

		FVector tempPoint = FVector();
		int tempPriority = 0;
		float tempDistance = 0.f;
		USceneComponent* tempComp = IIWorldInteractions::Execute_GetClosestComponentData(tempActor, this, tempPoint, tempPriority);
		tempDistance = (tempPoint - GetComponentLocation()).SizeSquared();

		if (tempPriority >= highestPriority)
		{
			//Do distance check
			if (tempPriority > highestPriority)
			{
				highestPriority = tempPriority;
				closestDistance = tempDistance;
				closestActor = tempActor;
				closestComponent = tempComp;
			}
			else
			{
				if (tempDistance <= closestDistance)
				{
					closestDistance = tempDistance;
					closestActor = tempActor;
					closestComponent = tempComp;
				}
			}
		}
	}

	foundActor = closestActor;
	foundComponent = closestComponent;

	if (bForceGrabbedActors && pParentPawn != nullptr)
	{
		TArray<FHitResult> hitResults;
		FVector traceStart = pOverlapShape->GetComponentLocation();
		FVector traceEnd = traceStart + (pParentPawn->pMotionControllerL->GetForwardVector() * pParentPawn->ForceGrabLength);
		FCollisionObjectQueryParams traceParams = FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects);
		FCollisionQueryParams queryParams = FCollisionQueryParams(FName(TEXT("ForceGrab_Trace")));
		queryParams.bTraceComplex = false;
		FCollisionShape collisionShape = FCollisionShape::MakeSphere(pParentPawn->ForceGrabRadius);

		if (GetWorld()->SweepMultiByObjectType(hitResults, traceStart, traceEnd, FQuat::Identity, traceParams, collisionShape, queryParams))
		{
			for (FHitResult& hitResult : hitResults)
			{
				if (hitResult.GetActor() == closestActor)
				{
					if (hitResult.GetActor() != nullptr)
					{
						bForceGrabbedActors = true;
						ImpactLocation = hitResult.ImpactPoint;
					}
				}
			}
		}
	}

	return closestActor;
}

void UWorldInteractor::ForceGrabActor(TArray<AActor*>& actors)
{
	if (pParentPawn == nullptr)
		return;

	actors.Reset();
	bForceGrabbedActors = false;
	TArray<FHitResult> hitResults;
	FVector traceStart = pOverlapShape->GetComponentLocation();
	FVector traceEnd = traceStart + (pParentPawn->pMotionControllerL->GetForwardVector() * pParentPawn->ForceGrabLength);
	FCollisionObjectQueryParams traceParams = FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects);
	FCollisionQueryParams queryParams = FCollisionQueryParams(FName(TEXT("ForceGrab_Trace")));
	queryParams.bTraceComplex = false;
	FCollisionShape collisionShape = FCollisionShape::MakeSphere(pParentPawn->ForceGrabRadius);

	if (GetWorld()->SweepMultiByObjectType(hitResults, traceStart, traceEnd, FQuat::Identity, traceParams, collisionShape, queryParams))
	{
		for (FHitResult& hitResult : hitResults)
		{
			actors.AddUnique(hitResult.GetActor());
			bForceGrabbedActors = true;
		}
	}

	//FHitResult hitResult;

	//if (GetWorld()->SweepSingleByObjectType(hitResult, traceStart, traceEnd, FQuat::Identity, traceParams, collisionShape, queryParams))
	//{
	//	actors.Add(hitResult.GetActor());
	//	bForceGrabbedActors = true;
	//}

	if (pParentPawn->bShowDebugForceGrab)
		DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor(255, 0, 0), false, 0.f, 0, pParentPawn->ForceGrabRadius);
}

void UWorldInteractor::UsePressed()
{
	if (pHoveredActor == nullptr)
		return;
	IIWorldInteractions::Execute_OnUsePressed(pHoveredActor, this);
}

void UWorldInteractor::UseReleased()
{
	if (pHoveredActor == nullptr)
		return;
	IIWorldInteractions::Execute_OnUseReleased(pHoveredActor, this);
}

void UWorldInteractor::PickupPressed()
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("PickupPressed"));*/
	if (pHoveredActor == nullptr || pOverlapShape == nullptr)
		return;
	if (bForceGrabbedActors)
	{
		if (pHoveredActor->GetRootComponent()->IsSimulatingPhysics())
		{
			FVector offset = ImpactLocation - pOverlapShape->GetComponentLocation();
			pHoveredActor->SetActorLocation(pHoveredActor->GetActorLocation() - offset);
		}
	}
	IIWorldInteractions::Execute_OnPickupPressed(pHoveredActor, this);
}

void UWorldInteractor::PickupReleased()
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("PickupReleased"));*/
	if (pHoveredActor == nullptr)
		return;
	IIWorldInteractions::Execute_OnPickupReleased(pHoveredActor, this);
}

void UWorldInteractor::DragStart()
{
	if (pHoveredActor == nullptr)
		return;
	IIWorldInteractions::Execute_OnDragStart(pHoveredActor, this);
	bDragging = true;
}

void UWorldInteractor::DragStop()
{
	if (pHoveredActor == nullptr)
		return;

	if (!bDragging)
		return;

	IIWorldInteractions::Execute_OnDragStop(pHoveredActor, this);
	bDragging = false;
}

void UWorldInteractor::SetHoveredActor(AActor* actor, bool lock)
{
	pHoveredActor = actor;
	//TO-DO: WORLD INTERACTIVE INTERFACE IMPLEMENTATION
	if (lock)
	{
		LockHoveredActor();
	}
	else
	{
		UnlockHoveredActor();
	}
}

void UWorldInteractor::LockHoveredActor()
{
	bHoveredActorLocked = true;
}

void UWorldInteractor::UnlockHoveredActor()
{
	bHoveredActorLocked = false;
}

UPrimitiveComponent* UWorldInteractor::GetOverlapShape()
{
	return pOverlapShape;
}

void UWorldInteractor::Setup(UPrimitiveComponent* overlapShape)
{
	pOverlapShape = overlapShape;
	this->SetActive(true, false);
}
