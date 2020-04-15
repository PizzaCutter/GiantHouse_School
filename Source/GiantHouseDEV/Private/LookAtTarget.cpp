// Fill out your copyright notice in the Description page of Project Settings.

#include "LookAtTarget.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALookAtTarget::ALookAtTarget(): TimeToLookAt{1.f},Timer{}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> foundMesh(TEXT("/Game/Art_Assets/BasicShapes/SM_Sphere.SM_Sphere"));
	if (foundMesh.Succeeded())
		pStaticMesh->SetStaticMesh(foundMesh.Object);

	SetActorScale3D(FVector(MinimumLerpScale, MinimumLerpScale, MinimumLerpScale));

	SetActorHiddenInGame(true);
}

void ALookAtTarget::OnTrace_Implementation(UTraceInteractor * interactor)
{
	if (bIsActive)
	{
		Timer += GetWorld()->DeltaTimeSeconds;
		float currentValue = FMath::Lerp(MinimumLerp, MaximumLerp, Timer / TimeToLookAt);
		float currentValueScale = FMath::Lerp(MinimumLerpScale, MaximumLerpScale, Timer / TimeToLookAt);
		pMaterial->SetScalarParameterValue("EmissiveIntensity", currentValue);
		SetActorScale3D(FVector(currentValueScale, currentValueScale, currentValueScale));
	}
	
}

void ALookAtTarget::OnTraceBegin_Implementation(UTraceInteractor* interactor)
{
	bIsBeingLookedAt = true;
	Timer = 0.f;
}

void ALookAtTarget::OnTraceEnd_Implementation(UTraceInteractor * interactor)
{
	bIsBeingLookedAt = false;
	Timer = 0.f;
	pMaterial->SetScalarParameterValue("EmissiveIntensity", MinimumLerp);
	SetActorScale3D(FVector(MinimumLerpScale, MinimumLerpScale, MinimumLerpScale));
}

// Called when the game starts or when spawned
void ALookAtTarget::BeginPlay()
{
	Super::BeginPlay();
	pMaterial = UMaterialInstanceDynamic::Create(pStaticMesh->GetMaterial(0), this);
	pStaticMesh->SetMaterial(0, pMaterial);
}

void ALookAtTarget::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	
	
}

// Called every frame
void ALookAtTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsBeingLookedAt && bIsActive)
	{
		if (Timer > TimeToLookAt && !IsEventTriggered())
		{
			if (pParticle != nullptr)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pParticle, GetTransform());
			}
			//UGameplayStatics::PlaySoundAtLocation(GetWorld(), pSound, GetTransform().GetLocation(),1.0f,FMath::RandRange(MinimumRandPitch,MaximumRandPitch));
			TriggerEvent();

		}
	}
}

