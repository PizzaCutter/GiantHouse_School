// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportationTrace.generated.h"

class UParticleSystem;
class UParticleSystemComponent;
class UArrowComponent;

UCLASS()
class GIANTHOUSEDEV_API ATeleportationTrace : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATeleportationTrace();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform & Transform);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	USkeletalMeshComponent* pSkeletalMeshComponent = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UParticleSystemComponent* pStar1 = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UParticleSystemComponent* pStar2 = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UParticleSystemComponent* pStar3 = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UParticleSystemComponent* pStar4 = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UParticleSystemComponent* pStar5 = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UParticleSystemComponent* pStarCenter = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UArrowComponent* EndPoint = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Default)
		float LerpTime = 1.f;
	float LerpTimer = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Default)
		float LifeTimeAfterLerp = 3.f;
	float TotalLifeTimer = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Default)
		float RotationSpeed = 2.f;

	void SetEndPointLocation(const FVector& newLocation);
};
