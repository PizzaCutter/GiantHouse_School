// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialEvent.h"
#include "InteractEvent.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class GIANTHOUSEDEV_API AInteractEvent : public ATutorialEvent
{
	GENERATED_BODY()
	
public:
	AInteractEvent();

	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform & Transform) override;

protected:
	virtual void BeginPlay() override final;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trigger", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* pBoxComp = nullptr;

	
};
