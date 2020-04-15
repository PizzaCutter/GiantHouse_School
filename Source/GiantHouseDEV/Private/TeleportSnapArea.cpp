// Fill out your copyright notice in the Description page of Project Settings.

#include "TeleportSnapArea.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATeleportSnapArea::ATeleportSnapArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	pStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	pStaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATeleportSnapArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATeleportSnapArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATeleportSnapArea::IsAreaActive()
{
	if (this->bHidden)
		return false;
	return true;
}

