// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Public/TraceInteractor.h"
#include "ITraceInteractions.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UITraceInteractions : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GIANTHOUSEDEV_API IITraceInteractions
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void OnTrace(UTraceInteractor* interactor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void OnTraceBegin(UTraceInteractor* interactor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void OnTraceEnd(UTraceInteractor* interactor);
};
