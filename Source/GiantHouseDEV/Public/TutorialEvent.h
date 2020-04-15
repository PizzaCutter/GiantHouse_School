// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialEvent.generated.h"

UCLASS()
class GIANTHOUSEDEV_API ATutorialEvent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialEvent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TraceInterface")
	void TriggerEvent();
	virtual void TriggerEvent_Implementation();
	bool IsEventTriggered();
	void SetUpEvent();
	void DisableEvent();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	bool bEventTriggered = false;
	bool bIsActive = false;

};
