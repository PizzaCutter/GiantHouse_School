// Fill out your copyright notice in the Description page of Project Settings.

#include "TeleportationTrace.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ATeleportationTrace::ATeleportationTrace()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	pSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	pStar1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Star1"));
	pStar2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Star2"));
	pStar3 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Star3"));
	pStar4 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Star4"));
	pStar5 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Star5"));
	pStarCenter = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("StarCenter"));
	EndPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("EndPoint"));

	pSkeletalMeshComponent->AttachTo(RootComponent);
	pStar1->AttachTo(pSkeletalMeshComponent, FName("Star01Socket"));
	pStar2->AttachTo(pSkeletalMeshComponent, FName("Star02Socket"));
	pStar3->AttachTo(pSkeletalMeshComponent, FName("Star03Socket"));
	pStar4->AttachTo(pSkeletalMeshComponent, FName("Star04Socket"));
	pStar5->AttachTo(pSkeletalMeshComponent, FName("Star05Socket"));
	pStarCenter->AttachTo(pSkeletalMeshComponent, FName("StarCenterSocket"));
	EndPoint->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void ATeleportationTrace::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATeleportationTrace::OnConstruction(const FTransform& Transform)
{
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(pSkeletalMeshComponent->GetComponentLocation(), EndPoint->GetComponentLocation());
	pSkeletalMeshComponent->SetWorldRotation(lookAtRotation);
	pSkeletalMeshComponent->AddLocalRotation(FRotator(90.f, 0.f, 0.f));
}

// Called every frame
void ATeleportationTrace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TotalLifeTimer += DeltaTime;

	if (LerpTimer < LerpTime)
	{
		LerpTimer += DeltaTime;
		FVector newLocation = FMath::Lerp(RootComponent->GetComponentLocation(), EndPoint->GetComponentLocation(), LerpTimer / LerpTime);
		if(pSkeletalMeshComponent != nullptr)
		{
			pSkeletalMeshComponent->SetWorldLocation(newLocation);
			pSkeletalMeshComponent->AddLocalRotation(FRotator(0.f, RotationSpeed, 0.f));
		}
	}

	if(TotalLifeTimer >= (LifeTimeAfterLerp + LerpTime))
	{
		Destroy();
	}
}

void ATeleportationTrace::SetEndPointLocation(const FVector& newLocation)
{
	if(EndPoint != nullptr)
		EndPoint->SetWorldLocation(newLocation);

	if (pSkeletalMeshComponent != nullptr)
	{
		FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(pSkeletalMeshComponent->GetComponentLocation(), EndPoint->GetComponentLocation());
		pSkeletalMeshComponent->SetWorldRotation(lookAtRotation);
		pSkeletalMeshComponent->AddLocalRotation(FRotator(90.f, 0.f, 0.f));
	}
}

