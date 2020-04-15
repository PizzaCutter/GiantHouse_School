// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
//#include "TeleportSnapArea.h"
#include "Teleporter.generated.h"

struct FPredictProjectilePathResult;
class UMotionControllerComponent;
class ABasePawn;
class ATeleportArea;
class ATeleportSnapArea;
class APlayerController;
class USplineComponent;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GIANTHOUSEDEV_API UTeleporter : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTeleporter();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Setup(ABasePawn* parentPawn);

	//Disables/Enables the component and Disabled/Enables the teleport areas in the level
	void SetActive(bool newState);
	bool IsActive() const
	{
		return bIsActive;
	}
	void SetAxisValues(FVector axisValue)
	{
		AxisValue = axisValue;
	}
	void Teleport();

	void ResetTeleportAreas();

private:
	bool bIsActive = false;
	//INPUT ROTATION
	FVector AxisValue = FVector();
	FRotator CalculatedRotation = FRotator();

	//REFERENCES TO PARENTS
	APlayerController* pPlayerController = nullptr;
	ABasePawn* pParentPawn = nullptr;
	UMotionControllerComponent* pMotionController = nullptr;

	int CurrentPoint = 0;
	//Time it takes for a point to fully light up and dim down
	float BlinkTimer = 0.f;

	float SnapRadius = 100.f;

	//REFERENCES TO TELEPORT AREAS
	TArray<ATeleportArea*> TeleportAreas;
	ATeleportArea* TargetTeleportArea = nullptr;
	TArray<ATeleportSnapArea*> TeleportSnapAreas;
	ATeleportSnapArea* TeleportSnapArea = nullptr;
	USplineComponent* pSnapArcSpline = nullptr;

	void FindAllTeleportAreas();
	void DisableAllTeleportAreas();
	void EnableAllTeleportAreas();

	//Returns TRUE if the arc hit a player area
	bool CreateArc(FPredictProjectilePathResult& pathResults);
	int FindTeleportSnapArea(FVector location);

	void VisualizeArc(float deltaTime);
	void VisualizeArcEnd(FPredictProjectilePathResult result, bool valid = true);
	void VisualizeArcPath(FPredictProjectilePathResult result, float deltaTime, bool valid = true);
	void DisableArcVisualization();

	void CalculateRotation();
};
