// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "BasePawn.generated.h"

class UCameraComponent;
class UMotionControllerComponent;
class UStaticMeshComponent;
class UWorldInteractor;
class UArrowComponent;
class UTeleporter;
class UTraceInteractor;
class UMaterialInstanceDynamic;
class USphereComponent;
class UParticleSystem;
class ATeleportationTrace;

UCLASS()
class GIANTHOUSEDEV_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void OnConstruction(const FTransform & Transform);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float axis);
	void MoveRight(float axis);

	void LeftThumbstickX(float axis);
	void LeftThumbstickY(float axis);
	void RightThumbstickX(float axis);
	void RightThumbstickY(float axis);

	void FindHMD();
	void CheckTeleport();
	void Trace(float deltaTime);

	UFUNCTION(BlueprintCallable)
	void ResetTeleporter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* pCamera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UMotionControllerComponent* pMotionControllerL = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UMotionControllerComponent* pMotionControllerR = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Grabbing)
		UStaticMeshComponent* pHandSphereL = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Grabbing)
		UStaticMeshComponent* pHandSphereR = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grabbing)
		bool bShowDebugForceGrab = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grabbing)
		float ForceGrabLength = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grabbing)
		float ForceGrabRadius = 10.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UWorldInteractor* pWorldInteractorL = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UWorldInteractor* pWorldInteractorR = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTraceInteractor* pTraceInteractor = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UArrowComponent* pControllerSocketL = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UArrowComponent* pControllerSocketR = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USphereComponent* pCameraCollider = nullptr;


	//-----------------------TELEPORTAITON-------------------------
	//TELEPORTATION COMPONENTS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Teleportation)
		bool bAllowTeleportation = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Teleportation)
		UTeleporter* pTeleporterL = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Teleportation)
		UTeleporter* pTeleporterR = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Teleportation)
		UStaticMeshComponent* pTeleportArrow = nullptr;
	UMaterialInstanceDynamic* pTeleportArrowDynamicMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		UStaticMesh* ArcTracePointMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		UMaterialInterface* pTracePointMaterial = nullptr;
	TArray<UStaticMeshComponent*> pTeleportTracePointsMeshes;
	TArray<UMaterialInstanceDynamic*> pTeleportTracePointsMaterials;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		UParticleSystem* pPreviousPositionParticle = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		TSubclassOf<class ATeleportationTrace>  TeleportationTrace;

	//TELEPORTATION PARAMS
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		float TeleportFadeTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		float TeleportArcVelocity = 900.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		float TracePointSize = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		FPredictProjectilePathParams ArcParams = FPredictProjectilePathParams();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		float BlinkTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		int BlinkSteps = 3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		float ArcPointMinEmissive = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		float ArcPointMaxEmissive = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Teleportation)
		bool bTeleportDisabled = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Teleportation)
		float TeleportationDelay = 1.f;
	float TeleportationDelayTimer = 0.f;

	
	void ArcPathMeshSetup();
	void AddMeshesToArcPathObjectPool(int count = 100);

	bool bDying = false;

	UFUNCTION(BlueprintCallable)
	void SetDying(bool newState)
	{
		bDying = newState;
	}
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDying()
	{
		return bDying;
	}

	//-------------------------DEV STUFF------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DEV")
		float MovementSpeed = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DEV")
		bool ForceKeyboardMouse = false;

	bool HasHMD = false;

	FVector leftThumbstick = FVector();
	FVector rightThumbstick = FVector();
	FVector prevLeftThumbstick = FVector();
	FVector prevRightThumbstick = FVector();
};
