// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelStreamer.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "Runtime/Engine/Classes/GameFramework/Pawn.h"
#include "Public/BasePawn.h"

// Sets default values
ALevelStreamer::ALevelStreamer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamer::OnOverlapBegin);
	OverlapVolume->OnComponentEndOverlap.AddDynamic(this, &ALevelStreamer::OnOverlapEnd);
}


void ALevelStreamer::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelStreamer::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	ABasePawn* CastedActor = Cast<ABasePawn>(OtherActor);
	if (CastedActor != nullptr)
	{
		if (LevelToLoad != "")
		{
			FLatentActionInfo LatentInfo;
			UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, false, LatentInfo);
		}
	}
	
}

void ALevelStreamer::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	ABasePawn* CastedActor = Cast<ABasePawn>(OtherActor);
	if (CastedActor != nullptr)
	{
		if (LevelToLoad != "")
		{
			FLatentActionInfo LatentInfo;
			UGameplayStatics::UnloadStreamLevel(this, LevelToLoad, LatentInfo,false);
		}
	}
}

// Called every frame
void ALevelStreamer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

