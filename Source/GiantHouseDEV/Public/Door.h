// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Public/IWorldInteractions.h"
#include "Door.generated.h"

class UWorldInteractor;

UCLASS()
class GIANTHOUSEDEV_API ADoor : public AActor, public IIWorldInteractions
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	USceneComponent* pDoorRotator = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UStaticMeshComponent* pDoor = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	UStaticMeshComponent* pDoorHandle = nullptr;

	FRotator InitialDoorRotation = FRotator();
	FRotator PreviousRotation = FRotator();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DoorPhysics")
	float InitialYawVelocity = 0.f;
	float InitialYawAngle = 0.f;
	float InitialGrabOffset = 0.f;
	float DoorTimeSinceLetGo = 0.f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DoorPhysics")
	bool bSimulateFakePhysics = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DoorPhysics")
	bool bJustBounced = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorPhysics")
		bool bLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorPhysics")
		bool bHoldOnForever = false;

	UFUNCTION(BlueprintCallable)
	void Lock()
	{
		bLocked = true;
	}
	UFUNCTION(BlueprintCallable)
	void Unlock()
	{
		bLocked = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorPhysics")
	float DoorFriction = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorPhysics")
		float BounceVelocityDiminisher = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorPhysics")
	float MinYaw = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorPhysics")
	float MaxYaw = 100.f;

	void StartSimulateDoorPhysics();
	void StopSimulateDoorPhysics();

	bool ShouldStopSimulation();
	float GetPhysicsRelativeYaw();

	//INTERFACE IMPLEMENTATIONS
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	bool OnDragStop(UWorldInteractor* interactor);
	virtual bool OnDragStop_Implementation(UWorldInteractor* interactor) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	bool OnDragStart(UWorldInteractor* interactor);
	virtual bool OnDragStart_Implementation(UWorldInteractor* interactor) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	bool OnDrag(UWorldInteractor* interactor);
	virtual bool OnDrag_Implementation(UWorldInteractor* interactor) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MyCategory")
	USceneComponent* GetClosestComponentData(UWorldInteractor* interactor, FVector& point, int& priority);
	virtual USceneComponent* GetClosestComponentData_Implementation(UWorldInteractor* interactor, FVector& point, int& priority) override;

	UWorldInteractor* pInteractor = nullptr;
};
