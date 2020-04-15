// Fill out your copyright notice in the Description page of Project Settings.

#include "Door.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

// Sets default values
ADoor::ADoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponnt"));
	pDoorRotator = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRotate"));
	pDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	pDoorHandle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorHandle"));

	pDoorRotator->SetupAttachment(RootComponent);
	pDoor->SetupAttachment(pDoorRotator);
	pDoorHandle->SetupAttachment(pDoor);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	InitialDoorRotation = FRotator(pDoorRotator->GetRelativeTransform().GetRotation());
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLocked)
		return;

	if (bSimulateFakePhysics)
	{
		DoorTimeSinceLetGo += GetWorld()->GetDeltaSeconds();
		if (ShouldStopSimulation())
		{
			StopSimulateDoorPhysics();
		}
		else
		{
			pDoorRotator->SetRelativeRotation(FRotator(0.f, GetPhysicsRelativeYaw(), 0.f));
		}
	}

	if (bHoldOnForever && pInteractor != nullptr)
	{
		InitialYawVelocity = pDoorRotator->RelativeRotation.Yaw - PreviousRotation.Yaw;
		InitialYawVelocity /= GetWorld()->GetDeltaSeconds();
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("OnDrag: %f"), InitialYawVelocity));

		FVector inversedLocation = UKismetMathLibrary::InverseTransformLocation(GetTransform(), pInteractor->GetComponentLocation());
		FVector tempLocation = inversedLocation - pDoorRotator->GetRelativeTransform().GetLocation();
		float tempAtan = FMath::Atan2(tempLocation.Y, tempLocation.X);
		tempAtan = FMath::RadiansToDegrees(tempAtan);
		float newRotation = tempAtan - InitialGrabOffset;
		newRotation = FMath::Clamp(newRotation, MinYaw, MaxYaw);

		PreviousRotation = FRotator(pDoorRotator->GetRelativeTransform().GetRotation());
		pDoorRotator->SetRelativeRotation(FRotator(0.f, newRotation, 0.f));
	}

}

void ADoor::StartSimulateDoorPhysics()
{
	InitialYawAngle = pDoorRotator->RelativeRotation.Yaw;
	DoorTimeSinceLetGo = 0.f;
	bSimulateFakePhysics = true;
	bJustBounced = false;
}

void ADoor::StopSimulateDoorPhysics()
{
	DoorTimeSinceLetGo = 0.f;
	bSimulateFakePhysics = false;
	bJustBounced = false;

}

bool ADoor::ShouldStopSimulation()
{
	bool slowedToStop = false;
	bool checkPastMin = false;
	bool checkPastMax = false;
	bool checkNearMin = false;
	bool checkNearMax = false;

	//SLOWED TO A STOP
	float temp = (DoorFriction * 980) * DoorTimeSinceLetGo * FMath::Sign(InitialYawVelocity);
	if (InitialYawVelocity <= 0.f)
	{
		slowedToStop = (InitialYawVelocity - temp) > 0.f;
		if(slowedToStop)
			InitialYawVelocity = 0.f;
	}
	else {
		slowedToStop = ((InitialYawVelocity - temp) <= 0.f);
		if (slowedToStop)
			InitialYawVelocity = 0.f;
	}

	//Door has reached the min or max yaw
	checkPastMin = pDoorRotator->RelativeRotation.Yaw >= MaxYaw;
	checkPastMax = pDoorRotator->RelativeRotation.Yaw <= MinYaw;

	checkNearMin = FMath::IsNearlyEqual(pDoorRotator->RelativeRotation.Yaw, MaxYaw, 1.f);
	checkNearMax = FMath::IsNearlyEqual(pDoorRotator->RelativeRotation.Yaw, MinYaw, 1.f);

	if (checkPastMin || checkPastMax || checkNearMin || checkNearMax)
	{
		if (bJustBounced == false)
		{
			bJustBounced = true;
			DoorTimeSinceLetGo = 0.f;
			InitialYawVelocity = InitialYawVelocity - temp;
			InitialYawVelocity = InitialYawVelocity * BounceVelocityDiminisher;
			InitialYawVelocity *= -1.f;

			if (checkPastMax || checkNearMax)
				InitialYawAngle = MinYaw;
			if (checkPastMin || checkNearMin)
				InitialYawAngle = MaxYaw;

			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, TEXT("BOUNCE"));
		}
	}

	return slowedToStop;// || checkPastMin || checkPastMax || checkNearMin || checkNearMax;
}

float ADoor::GetPhysicsRelativeYaw()
{
	float temp1 = InitialYawVelocity * DoorTimeSinceLetGo;
	float temp2 = 0.5 * DoorFriction * 980 * FMath::Pow(DoorTimeSinceLetGo, 2) * FMath::Sign(InitialYawVelocity);
	float temp3 = temp1 - temp2;
	float temp4 = InitialYawAngle + temp3;

	bool checkNearMin = FMath::IsNearlyEqual(temp4, MaxYaw, 1.f);
	bool checkNearMax = FMath::IsNearlyEqual(temp4, MinYaw, 1.f);

	if (bJustBounced)
	{
		if (temp4 > MinYaw && temp4 < MaxYaw)// && checkNearMin && checkNearMax)
			bJustBounced = false;
	}
	return FMath::ClampAngle(temp4, MinYaw, MaxYaw);
}

bool ADoor::OnDragStop_Implementation(UWorldInteractor * interactor)
{
	if (interactor == nullptr)
		return false;

	if (!bHoldOnForever)
	{
		InitialYawVelocity = pDoorRotator->RelativeRotation.Yaw - PreviousRotation.Yaw;
		InitialYawVelocity /= GetWorld()->GetDeltaSeconds();
		interactor->UnlockHoveredActor();

		StartSimulateDoorPhysics();
	}

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT("OnDragStop: %f"), InitialYawVelocity));
	return false;
}

bool ADoor::OnDragStart_Implementation(UWorldInteractor * interactor)
{
	if (bLocked)
		return false;

	if (interactor == nullptr)
		return false;

	if(bHoldOnForever)
	{
		if(!bSimulateFakePhysics)
		{
			FVector inversedLocation = UKismetMathLibrary::InverseTransformLocation(GetTransform(), interactor->GetComponentLocation());
			FVector tempLocation = inversedLocation - pDoorRotator->GetRelativeTransform().GetLocation();
			float tempAtan = FMath::Atan2(tempLocation.Y, tempLocation.X);
			tempAtan = FMath::RadiansToDegrees(tempAtan);
			InitialGrabOffset = tempAtan - pDoorRotator->RelativeRotation.Yaw;
			interactor->LockHoveredActor();

			StopSimulateDoorPhysics();

			pInteractor = interactor;
		}
	}else
	{
		FVector inversedLocation = UKismetMathLibrary::InverseTransformLocation(GetTransform(), interactor->GetComponentLocation());
		FVector tempLocation = inversedLocation - pDoorRotator->GetRelativeTransform().GetLocation();
		float tempAtan = FMath::Atan2(tempLocation.Y, tempLocation.X);
		tempAtan = FMath::RadiansToDegrees(tempAtan);
		InitialGrabOffset = tempAtan - pDoorRotator->RelativeRotation.Yaw;
		interactor->LockHoveredActor();

		StopSimulateDoorPhysics();
	}

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("OnDragStart"));
	return false;
}

bool ADoor::OnDrag_Implementation(UWorldInteractor* interactor)
{
	if (bLocked)
		return false;

	if (interactor == nullptr)
		return false;

	if (!bHoldOnForever)
	{
		InitialYawVelocity = pDoorRotator->RelativeRotation.Yaw - PreviousRotation.Yaw;
		InitialYawVelocity /= GetWorld()->GetDeltaSeconds();
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("OnDrag: %f"), InitialYawVelocity));

		FVector inversedLocation = UKismetMathLibrary::InverseTransformLocation(GetTransform(), interactor->GetComponentLocation());
		FVector tempLocation = inversedLocation - pDoorRotator->GetRelativeTransform().GetLocation();
		float tempAtan = FMath::Atan2(tempLocation.Y, tempLocation.X);
		tempAtan = FMath::RadiansToDegrees(tempAtan);
		float newRotation = tempAtan - InitialGrabOffset;
		newRotation = FMath::Clamp(newRotation, MinYaw, MaxYaw);

		PreviousRotation = FRotator(pDoorRotator->GetRelativeTransform().GetRotation());
		pDoorRotator->SetRelativeRotation(FRotator(0.f, newRotation, 0.f));
	}

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, TEXT("OnDrag"));
	return false;
}

//INTERFACE FUNCTION IMPLEMENTATIONS
USceneComponent* ADoor::GetClosestComponentData_Implementation(UWorldInteractor* interactor, FVector& point, int& priority)
{
	if (bLocked)
		return nullptr;

	pDoorHandle->GetClosestPointOnCollision(interactor->GetComponentLocation(), point, FName(TEXT("NONE")));
	priority = 0;
	return pDoorHandle;
}