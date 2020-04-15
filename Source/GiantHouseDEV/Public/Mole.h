// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mole.generated.h"

class USkeletalMeshComponent;

UCLASS()
class GIANTHOUSEDEV_API AMole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMole();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Default)
	USkeletalMeshComponent* pMoleSkeletalMesh = nullptr;

};
