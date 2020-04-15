// Fill out your copyright notice in the Description page of Project Settings.

#include "TutorialEvent.h"
#include "Engine/Engine.h"
// Sets default values
ATutorialEvent::ATutorialEvent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATutorialEvent::TriggerEvent_Implementation()
{
	bEventTriggered = true;
	SetActorHiddenInGame(true);
	bIsActive = false;
}

bool ATutorialEvent::IsEventTriggered()
{
	return bEventTriggered;
}

void ATutorialEvent::SetUpEvent()
{
	SetActorHiddenInGame(false);
	bEventTriggered = false;
	bIsActive = true;
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("EventSetUp"));
}

void ATutorialEvent::DisableEvent()
{
	SetActorHiddenInGame(true);
	bEventTriggered = false;
	bIsActive = false;
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Event Disabled"));
}

// Called when the game starts or when spawned
void ATutorialEvent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATutorialEvent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

