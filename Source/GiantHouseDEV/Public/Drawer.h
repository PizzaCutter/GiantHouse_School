// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Public/IWorldInteractions.h"
#include "Drawer.generated.h"

UCLASS()
class GIANTHOUSEDEV_API ADrawer : public AActor, public IIWorldInteractions
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADrawer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
	//	USceneComponent* pDrawer = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
		UStaticMeshComponent* pDrawer = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Default)
		UStaticMeshComponent* pDrawerHandle = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DoorPhysics")
	FVector InitialLocation = FVector();
	FVector PreviousLocation = FVector();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DoorPhysics")
		float InitialVelocity = 0.f;
	float InitialPos = 0.f;
	float InitialGrabOffset = 0.f;
	float TimeSinceLetGo = 0.f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DoorPhysics")
	bool bSimulateFakePhysics = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DoorPhysics")
	bool bJustBounced = false;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "DoorPhysics")
		bool bIsBeingDragged = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorPhysics")
		float DrawerFriction = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorPhysics")
		float BounceVelocityDiminisher = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoorPhysics")
		float DrawerLength = 100.f;

	void StartSimulateDoorPhysics();
	void StopSimulateDoorPhysics();

	bool ShouldStopSimulation();
	float GetPhysicsRelativeLocation();

	//-----------------------------INTERFACE IMPLEMENTATIONS-----------------------------
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
};
