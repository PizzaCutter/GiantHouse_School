// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "WorldInteractor.generated.h"

class UPrimitiveComponent;
class ABasePawn;

UENUM(BlueprintType)
enum class EHand : uint8
{
	E_Left UMETA(DisplayName = "Left Hand"),
	E_Right UMETA(DisplayName = "Right Hand")
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GIANTHOUSEDEV_API UWorldInteractor : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWorldInteractor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void Setup(UPrimitiveComponent* overlapShape);
	AActor* FindActorToHover(TArray<AActor*> actors, AActor* foundActor, USceneComponent* foundComponent);
	void ForceGrabActor(TArray<AActor*>& actors);

	void UsePressed();
	void UseReleased();

	UFUNCTION()
	void PickupPressed();
	void PickupReleased();

	void DragStart();
	void DragStop();

	void SetHoveredActor(AActor* actor, bool lock);

	UFUNCTION(BlueprintCallable)
	void LockHoveredActor();
	UFUNCTION(BlueprintCallable)
	void UnlockHoveredActor();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPrimitiveComponent* GetOverlapShape();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	float InteractionRadius = 0.f;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	AActor* pHoveredActor = nullptr;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	bool bDragging = false;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	EHand eHAND = EHand::E_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	bool bHoveredActorLocked = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	bool bDebugDraw = false;

	bool bForceGrabbedActors = false;
	FVector ImpactLocation = FVector();

	USceneComponent* pHoveredComponent = nullptr;
	UPrimitiveComponent* pOverlapShape = nullptr;
	ABasePawn* pParentPawn = nullptr;
};
