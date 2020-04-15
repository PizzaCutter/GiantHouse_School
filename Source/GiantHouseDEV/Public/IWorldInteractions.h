// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Public/WorldInteractor.h"
#include "IWorldInteractions.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIWorldInteractions : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GIANTHOUSEDEV_API IIWorldInteractions
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	////---------------------------EXAMPLES---------------------------
	////classes using this interface must implement ReactToHighNoon
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyCategory")
	//	bool ReactToHighNoon();

	////classes using this interface may implement ReactToMidnight
	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MyCategory")
	//	bool ReactToMidnight();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
		bool OnHover(UWorldInteractor* interactor);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
		bool OnUnhover(UWorldInteractor* interactor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
		bool OnUsePressed(UWorldInteractor* interactor);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
		bool OnUseReleased(UWorldInteractor* interactor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
		bool OnPickupPressed(UWorldInteractor* interactor);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
		bool OnPickupReleased(UWorldInteractor* interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		bool OnDrag(UWorldInteractor* interactor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		bool OnDragStart(UWorldInteractor* interactor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		bool OnDragStop(UWorldInteractor* interactor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		USceneComponent* GetClosestComponentData(UWorldInteractor* interactor, FVector& point, int& priority);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
		bool OnHoveredComponentChanged(UWorldInteractor* interactor, USceneComponent* oldComponent, USceneComponent* newComponent);
};
