// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TraceInteractor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GIANTHOUSEDEV_API UTraceInteractor : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTraceInteractor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Trace();

	AActor* pHitActor = nullptr;
};
