// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialEvent.h"
#include "TeleportTarget.generated.h"

class UBoxComponent;
class ATeleportArea;
class UStaticMeshComponent;
class ABasePawn;

/**
 * 
 */
UCLASS()
class GIANTHOUSEDEV_API ATeleportTarget : public ATutorialEvent
{
	GENERATED_BODY()

public:
	ATeleportTarget();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform & Transform) override;

protected:
	virtual void BeginPlay() override final;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CheckTeleport();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* pBoxComp = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
		ATeleportArea* pArea = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
		bool bWithRotation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
		float DesiredRotation = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
		float Tolerance = 5.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* pTeleportDirection = nullptr;
	

private:
	ABasePawn * pOverlappedActor = nullptr;

};
