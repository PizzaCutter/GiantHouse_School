// Fill out your copyright notice in the Description page of Project Settings.

#include "TeleportTarget.h"
#include "Components/BoxComponent.h"
#include "TeleportArea.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "BasePawn.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

ATeleportTarget::ATeleportTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	pBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	pBoxComp->SetupAttachment(RootComponent);
	pTeleportDirection = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportArrow"));
	//pTeleportDirection->SetRelativeLocation(GetActorLocation());
	pTeleportDirection->SetupAttachment(RootComponent);

	pBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ATeleportTarget::OnOverlapBegin);
	pBoxComp->OnComponentEndOverlap.AddDynamic(this, &ATeleportTarget::OnOverlapEnd);
}


void ATeleportTarget::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	if (pOverlappedActor != nullptr)
	{
		if (bWithRotation)
		{
			if (pOverlappedActor == nullptr)
				return;
			if (pOverlappedActor->pTeleportArrow == nullptr)
				return;
			if (pTeleportDirection == nullptr)
				return;

			float teleportArrowYawRotation = pOverlappedActor->pTeleportArrow->GetComponentRotation().Yaw;
			float neededYawRotation = pTeleportDirection->GetComponentRotation().Yaw;
			float difference = (abs(teleportArrowYawRotation - neededYawRotation));
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Difference: %f"), difference));
			if (difference <= Tolerance)
			{
				pArea->bAbleToTeleport = true;
			}
			else
			{
				pArea->bAbleToTeleport = false;
			}
		}else
		{
			pArea->bAbleToTeleport = true;
		}
	}
	CheckTeleport();
}

void ATeleportTarget::OnConstruction(const FTransform & Transform)
{

}

void ATeleportTarget::BeginPlay()
{
	Super::BeginPlay();

	if (pArea != nullptr)
		pArea->bIsEnabled = true;

	if (bWithRotation && pTeleportDirection != nullptr)
		pTeleportDirection->SetWorldRotation(FRotator(0, DesiredRotation, 0));

	if (pArea != nullptr)
		pArea->bAbleToTeleport = false;
}

void ATeleportTarget::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	ABasePawn* castedActor = Cast<ABasePawn>(OtherActor);
	if (castedActor != nullptr)
	{
		pOverlappedActor = castedActor;
	}
}

void ATeleportTarget::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ABasePawn* castedActor = Cast<ABasePawn>(OtherActor);
	if (castedActor == pOverlappedActor)
	{
		//CheckTeleport();
		pOverlappedActor = nullptr;
	}
}

void ATeleportTarget::CheckTeleport()
{
	if(pArea != nullptr)
	{
		if (pArea->bTeleportedTo && pArea->bAbleToTeleport)
		{
			TriggerEvent();
			if (pArea != nullptr)
				pArea->bIsEnabled = false;
			pOverlappedActor = nullptr;
		}
	}
}
