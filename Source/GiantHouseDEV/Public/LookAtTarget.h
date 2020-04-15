// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/TutorialEvent.h"
#include "Public/ITraceInteractions.h"
#include "LookAtTarget.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UParticleSystem;
class USoundBase;


UCLASS()
class GIANTHOUSEDEV_API ALookAtTarget : public ATutorialEvent, public IITraceInteractions
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALookAtTarget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TraceInterface")
	void OnTrace(UTraceInteractor* interactor);
	virtual void OnTrace_Implementation(UTraceInteractor* interactor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TraceInterface")
	void OnTraceBegin(UTraceInteractor* interactor);
	virtual void OnTraceBegin_Implementation(UTraceInteractor* interactor) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TraceInterface")
		void OnTraceEnd(UTraceInteractor* interactor);
	virtual void OnTraceEnd_Implementation(UTraceInteractor* interactor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform & Transform) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visuals")
	UMaterialInstanceDynamic* pMaterial = nullptr;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* pStaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* pParticle = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
		USoundBase* pSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals",  meta = (AllowPrivateAccess = "true"))
		float TimeToLookAt = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
		float MinimumLerp = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
		float MaximumLerp = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
		float MinimumLerpScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals", meta = (AllowPrivateAccess = "true"))
		float MaximumLerpScale = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
		float MinimumRandPitch = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
		float MaximumRandPitch = 2.0f;


private:

	float Timer = 0.f;
	bool bIsBeingLookedAt = false;
};