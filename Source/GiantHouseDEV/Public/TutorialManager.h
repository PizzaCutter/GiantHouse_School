// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

class ATutorialEvent;
class ATeleportArea;

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EToSelect : uint8
{
	TS_JoystickL	UMETA(DisplayName = "JoyStickL"),
	TS_GripL		UMETA(DisplayName = "GripL"),
	TS_TriggerL		UMETA(DisplayName = "TriggerL"),
	TS_XL			UMETA(DisplayName = "XL"),
	TS_YL			UMETA(DisplayName = "YL"),
	TS_OptionsL		UMETA(DisplayName = "OptionsL"),
	TS_JoystickR	UMETA(DisplayName = "JoyStickR"),
	TS_GripR 		UMETA(DisplayName = "GripR"),
	TS_TriggerR		UMETA(DisplayName = "TriggerR"),
	TS_XR			UMETA(DisplayName = "XR"),
	TS_YR			UMETA(DisplayName = "YR"),
	TS_OptionsR		UMETA(DisplayName = "OptionsR")

};

USTRUCT(BlueprintType)
struct FSequence
{
	GENERATED_BODY()

public:




	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events", meta = (AllowPrivateAccess = "true"))
		bool bDisableTeleport;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events", meta = (AllowPrivateAccess = "true"))
		TArray<ATutorialEvent*> pSequence;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events", meta = (AllowPrivateAccess = "true"))
		TArray<AActor*> pExtras;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events", meta = (AllowPrivateAccess = "true"))
		TArray<ATeleportArea*> pAreasToDisable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events", meta = (AllowPrivateAccess = "true"))
		TArray<EToSelect> pButtonsToLightUp;
};


UCLASS()
class GIANTHOUSEDEV_API ATutorialManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	
	
	ATutorialManager();
	UFUNCTION(BlueprintCallable)
	void DisableTutorial();
	void SetUpSequence();
	bool CheckSequenceCompleted();
	void SetUpExtras();
	void SetUpAreas();
	void ReactivateAreas();
	void DisablePreviousExtras();
	void Finished();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TraceInterface")
	void ResetEmmisive();
	virtual void ResetEmmisive_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TraceInterface")
	void SetUpEmmisive();
	virtual void SetUpEmmisive_Implementation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events", meta = (AllowPrivateAccess = "true"))
	TArray<FSequence> pEventSequences;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Events", meta = (AllowPrivateAccess = "true"))
	int CurrentEventSequence = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bFinished = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bFinishedSequence = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bDisableTutorial = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events", meta = (AllowPrivateAccess = "true"))
		TArray<AActor*> pObjectsToDeleteAfterTutorial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events", meta = (AllowPrivateAccess = "true"))
		TArray<AActor*> pObjectsToEnableAfterFinished;
private:
};
