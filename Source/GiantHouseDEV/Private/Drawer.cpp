// Fill out your copyright notice in the Description page of Project Settings.

#include "Drawer.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

// Sets default values
ADrawer::ADrawer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponnt"));
	pDrawer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	pDrawerHandle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorHandle"));

	pDrawer->SetupAttachment(RootComponent);
	pDrawerHandle->SetupAttachment(pDrawer);
}

// Called when the game starts or when spawned
void ADrawer::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = pDrawer->GetRelativeTransform().GetLocation();
}

// Called every frame
void ADrawer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bSimulateFakePhysics)
	{
		TimeSinceLetGo += GetWorld()->GetDeltaSeconds();
		if(ShouldStopSimulation())
		{
			StopSimulateDoorPhysics();
		}else
		{
			float PhysicsLocation = GetPhysicsRelativeLocation();
			if(pDrawer != nullptr)
				pDrawer->SetRelativeLocation(FVector(PhysicsLocation, 0.f, 0.f));
		}
	}
}

void ADrawer::StartSimulateDoorPhysics()
{
	InitialPos = pDrawer->GetRelativeTransform().GetLocation().X;
	TimeSinceLetGo = 0.f;
	bSimulateFakePhysics = true;
	bJustBounced = false;
}

void ADrawer::StopSimulateDoorPhysics()
{
	TimeSinceLetGo = 0.f;
	bSimulateFakePhysics = false;
	bJustBounced = false;
}

bool ADrawer::ShouldStopSimulation()
{
	bool slowedToStop = false;
	bool checkPastMin = false;
	bool checkPastMax = false;

	float temp = (DrawerFriction * 980) * TimeSinceLetGo * FMath::Sign(InitialVelocity);
	if(InitialVelocity <= 0.f)
	{
		slowedToStop = (InitialVelocity - temp) > 0.f;
		if (slowedToStop)
		{
			InitialVelocity = 0.f;
			//return true;
		}
	}else
	{
		slowedToStop = (InitialVelocity - temp) <= 0.f;
		if (slowedToStop)
		{
			InitialVelocity = 0.f;
			//return true;
		}
	}

	//Door has reached the min or max location
	checkPastMin = pDrawer->GetRelativeTransform().GetLocation().X >= InitialLocation.X + DrawerLength;
	checkPastMax = pDrawer->GetRelativeTransform().GetLocation().X <= InitialLocation.X;

	if(checkPastMin || checkPastMax)
	{
		if(bJustBounced == false)
		{
			bJustBounced = true;
			TimeSinceLetGo = 0.f;
			InitialVelocity = InitialVelocity - temp;
			InitialVelocity = InitialVelocity * BounceVelocityDiminisher;
			InitialVelocity *= -1.f;

			if (checkPastMin)
				InitialPos = InitialLocation.X + DrawerLength;
			if (checkPastMax)
				InitialPos = InitialLocation.X;

			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, TEXT("BOUNCE"));
		}
	}

	return slowedToStop;// || checkPastMin || checkPastMax;
}

float ADrawer::GetPhysicsRelativeLocation()
{
	float temp1 = InitialVelocity * TimeSinceLetGo;
	float temp2 = 0.5f * DrawerFriction * 980 * FMath::Pow(TimeSinceLetGo, 2) * FMath::Sign(InitialVelocity);
	float temp3 = temp1 - temp2;
	float temp4 = InitialPos + temp3;

	if(bJustBounced)
	{
		if (temp4 > InitialLocation.X && temp4 < InitialLocation.X + DrawerLength)
			bJustBounced = false;
	}
	return FMath::Clamp(temp4, InitialLocation.X, InitialLocation.X + DrawerLength);
}

bool ADrawer::OnDragStop_Implementation(UWorldInteractor * interactor)
{
	//CALCULATE THE VELOCITY EVERY FRAME
	InitialVelocity = pDrawer->GetRelativeTransform().GetLocation().X - PreviousLocation.X;
	InitialVelocity /= GetWorld()->GetDeltaSeconds();
	interactor->UnlockHoveredActor();

	StartSimulateDoorPhysics();

	bIsBeingDragged = false;
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT("OnDragStop: %f"), InitialVelocity));
	return false;
}

bool ADrawer::OnDragStart_Implementation(UWorldInteractor * interactor)
{
	interactor->LockHoveredActor();
	InitialVelocity = 0.f;
	const FVector inversedLocation = UKismetMathLibrary::InverseTransformLocation(GetTransform(), interactor->GetComponentLocation());
	const FVector tempLocation = inversedLocation - pDrawer->GetRelativeTransform().GetLocation();
	InitialGrabOffset = tempLocation.X;

	StopSimulateDoorPhysics();

	bIsBeingDragged = true;
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("OnDragStart"));
	return false;
}

bool ADrawer::OnDrag_Implementation(UWorldInteractor* interactor)
{
	//Calculate the velocity every frame
	InitialVelocity = pDrawer->GetRelativeTransform().GetLocation().X - PreviousLocation.X;
	InitialVelocity /= GetWorld()->GetDeltaSeconds();

	//Cache the previous location
	PreviousLocation = pDrawer->GetRelativeTransform().GetLocation();

	//Transform the interactor to local space
	const FVector inversedLocation = UKismetMathLibrary::InverseTransformLocation(GetTransform(), interactor->GetComponentLocation());
	pDrawer->SetRelativeLocation(FVector(FMath::Clamp(inversedLocation.X - InitialGrabOffset, InitialLocation.X, InitialLocation.X + DrawerLength), InitialLocation.Y, InitialLocation.Z));
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, TEXT("OnDrag"));
	return false;
}

//INTERFACE FUNCTION IMPLEMENTATIONS
USceneComponent* ADrawer::GetClosestComponentData_Implementation(UWorldInteractor* interactor, FVector& point, int& priority)
{
	pDrawer->GetClosestPointOnCollision(interactor->GetComponentLocation(), point, FName(TEXT("NONE")));
	//point = GetActorLocation();
	priority = 0;
	return pDrawer;
}