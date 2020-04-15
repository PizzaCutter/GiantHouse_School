// Fill out your copyright notice in the Description page of Project Settings.

#include "TutorialManager.h"
#include "TutorialEvent.h"
#include "Kismet/GameplayStatics.h"
#include "BasePawn.h"
#include "Engine/GameEngine.h"
#include "TeleportArea.h"

// Sets default values
ATutorialManager::ATutorialManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATutorialManager::DisableTutorial()
{
	bDisableTutorial = true;


	if(pEventSequences.Num() != 0 && CurrentEventSequence < pEventSequences.Num())
	{
		if(pEventSequences[CurrentEventSequence].pSequence.Num() != 0)
		{
			for (auto current : pEventSequences[CurrentEventSequence].pSequence)
			{
				if (current != nullptr)
				{
					ATutorialEvent* currentCasted = Cast<ATutorialEvent>(current);
					if (currentCasted != nullptr)
					{
						currentCasted->DisableEvent();
					}
				}
			}
		}
	}

	if (pEventSequences.Num() != 0 && CurrentEventSequence < pEventSequences.Num())
	{
		if (pEventSequences[CurrentEventSequence].pAreasToDisable.Num() != 0)
		{
			for (auto current : pEventSequences[CurrentEventSequence].pAreasToDisable)
			{
				if (current != nullptr)
				{
					current->bIsEnabled = true;
				}
			}
		}
	}

	if (pEventSequences.Num() != 0 && CurrentEventSequence < pEventSequences.Num())
	{
		if (pEventSequences[CurrentEventSequence].pExtras.Num() != 0)
		{
			for (auto current : pEventSequences[CurrentEventSequence].pExtras)
			{
				if (current != nullptr)
				{
					current->SetActorHiddenInGame(true);
				}
			}
		}
	}

	ResetEmmisive();

	APawn* player = UGameplayStatics::GetPlayerController(this, 0)->GetPawn();
	if(player != nullptr)
	{
		ABasePawn* castedPawn = Cast<ABasePawn>(player);
		if (castedPawn != nullptr)
		{
			castedPawn->bTeleportDisabled = false;
		}
	}

	Finished();
}

void ATutorialManager::SetUpSequence()
{
	if (pEventSequences.Num() != 0)
	{
		if (pEventSequences[CurrentEventSequence].pSequence.Num() != 0)
		{
			for (auto current : pEventSequences[CurrentEventSequence].pSequence)
			{
				if (current != nullptr)
				{
					ATutorialEvent* currentCasted = Cast<ATutorialEvent>(current);
					if (currentCasted != nullptr)
					{
						currentCasted->SetUpEvent();
					}
				}
			}

			APawn* player = UGameplayStatics::GetPlayerController(this, 0)->GetPawn();
			ABasePawn* castedPawn = Cast<ABasePawn>(player);
			if (castedPawn != nullptr)
			{
				castedPawn->bTeleportDisabled = pEventSequences[CurrentEventSequence].bDisableTeleport;
			}


		}
	}



}

bool ATutorialManager::CheckSequenceCompleted()
{
	if (pEventSequences.Num() != 0 && pEventSequences[CurrentEventSequence].pSequence.Num() != 0)
	{
		for (auto current : pEventSequences[CurrentEventSequence].pSequence)
		{
			if (current != nullptr)
			{
				ATutorialEvent* currentCasted = Cast<ATutorialEvent>(current);
				if (currentCasted != nullptr)
				{
					if (!currentCasted->IsEventTriggered()) 
						return false;
				}
			}
		}

	}

	bFinishedSequence = true;
	return true;
}

void ATutorialManager::SetUpExtras()
{
	if (pEventSequences.Num() != 0 && pEventSequences[CurrentEventSequence].pExtras.Num() != 0)
	{
		for (auto current : pEventSequences[CurrentEventSequence].pExtras)
		{
			if (current != nullptr)
			{
				current->SetActorHiddenInGame(false);
			}
		}
	}
}

void ATutorialManager::SetUpAreas()
{
	if (pEventSequences.Num() != 0 && pEventSequences[CurrentEventSequence].pAreasToDisable.Num() != 0)
	{
		for (auto current : pEventSequences[CurrentEventSequence].pAreasToDisable)
		{
			if (current != nullptr)
			{
				current->bIsEnabled = false;
			}
		}
	}
}

void ATutorialManager::ReactivateAreas()
{
	if (pEventSequences.Num() != 0 && pEventSequences[CurrentEventSequence - 1].pAreasToDisable.Num() != 0)
	{
		for (auto current : pEventSequences[CurrentEventSequence - 1].pAreasToDisable)
		{
			if (current != nullptr)
			{
				current->bIsEnabled = true;
			}
		}
	}
}

void ATutorialManager::DisablePreviousExtras()
{
	if (pEventSequences.Num() != 0 && pEventSequences[CurrentEventSequence - 1].pExtras.Num() != 0)
	{
		for (auto current : pEventSequences[CurrentEventSequence - 1].pExtras)
		{
			if (current != nullptr)
			{
				current->SetActorHiddenInGame(true);
			}
		}
	}
}

void ATutorialManager::Finished()
{
	for (int i = 0; i < pObjectsToDeleteAfterTutorial.Num(); i++)
	{
		AActor* objectToDelete = pObjectsToDeleteAfterTutorial[i];
		if(objectToDelete != nullptr)
		{
			if (!IsValid(objectToDelete))
				continue;
			objectToDelete->Destroy(objectToDelete);
			i--;
		}
	}

	for (int i = 0; i < pObjectsToEnableAfterFinished.Num(); i++)
	{
		AActor* objectToEnable = pObjectsToEnableAfterFinished[i];
		if (objectToEnable != nullptr)
		{
			if (!IsValid(objectToEnable))
				continue;
			objectToEnable->SetActorHiddenInGame(false);
		}
	}

}

void ATutorialManager::ResetEmmisive_Implementation()
{
}

void ATutorialManager::SetUpEmmisive_Implementation()
{
}


// Called when the game starts or when spawned
void ATutorialManager::BeginPlay()
{
	Super::BeginPlay();
	if (!bDisableTutorial)
	{
		SetUpSequence();
		SetUpExtras();
		SetUpAreas();
		SetUpEmmisive();
	}
}

// Called every frame
void ATutorialManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bDisableTutorial)
	{
		if (!bFinished)
		{
			if (CheckSequenceCompleted())
			{
				CurrentEventSequence++;

				DisablePreviousExtras();
				ReactivateAreas();
				ResetEmmisive();

				if (CurrentEventSequence < pEventSequences.Num())
				{
					SetUpEmmisive();
					SetUpSequence();
					SetUpExtras();
					SetUpAreas();
				}
				else
				{
					bFinished = true;
					Finished();
				}
			}
		}
		else
		{
			APawn* player = UGameplayStatics::GetPlayerController(this, 0)->GetPawn();
			if(player != nullptr)
			{
				ABasePawn* castedPawn = Cast<ABasePawn>(player);
				if (castedPawn != nullptr)
				{
					castedPawn->bTeleportDisabled = false;
				}
			}
		}
	}
}

