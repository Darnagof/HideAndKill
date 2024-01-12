// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "HideAndKillGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "HideAndKill/NPC.h"


ASpawnManager::ASpawnManager()
{
	bNetLoadOnClient = false;
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawnManager::BeginPlay()
{
}

void ASpawnManager::InitManager()
{
	GM = GetWorld()->GetAuthGameMode<AHideAndKillGameMode>();
	if (!GM)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is not HideAndKillGameMode : Spawn Manager won't works"));
		return;
	}
	// Init AI Controllers
	AIControllers.Reserve(MaxNPCPopulation);
	TSubclassOf<AController> AIControllerClass = GM->DefaultAIPawnClass->GetDefaultObject<APawn>()->AIControllerClass; // Use Controller class of default AI Pawn defined in Game Mode
	FActorSpawnParameters SpawnParams;

	//Skin Types;
	int typeOne = 1;
	int typeTwo = 2; 
	int typeThree = 3;

	// Get Controllers from map placed NPC
	TArray<AActor*> SpawnedAIControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIControllerClass, SpawnedAIControllers);
	for (auto SpawnedAIController : SpawnedAIControllers)
	{
		AAIController* FoundController = Cast<AAIController>(SpawnedAIController);
		if (AIControllers.Num() < MaxNPCPopulation)
		{
			AIControllers.Add(FoundController);

			//Spread equal amount of colors amongst AI population	
			ANPC* NPC = Cast<ANPC>(FoundController->K2_GetPawn());
			NPC->AssignSkin();
		}
		else
		{
			// If max population is already reached, remove excess NPCs
			APawn* ControllerPawn = FoundController->GetPawn();
			FoundController->UnPossess();
			ControllerPawn->Destroy();
			FoundController->Destroy();
		}
	}

	// Create controllers to fit max NPC population
	for (int i = AIControllers.Num(); i < MaxNPCPopulation; i++)
	{
		AAIController* NewController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, SpawnParams);
		AIControllers.Add(NewController);
	}

	// Spawn all NPC
	SpawnAllNPC();
}

void ASpawnManager::OnNPCKilled(AAIController* NPC, APlayerController* Killer)
{
	if (NPC)
	{
		NPC->UnPossess(); // TODO call that here ?
		GM->RestartPlayer(NPC);
	}
	UE_LOG(LogTemp, Display, TEXT("NPC respawned"));
}

void ASpawnManager::SpawnAllNPC()
{
	for (auto Controller : AIControllers)
	{
		GM->RestartPlayer(Controller.Get());
	}
}

void ASpawnManager::OnPlayerKilled(APlayerController* Player, APlayerController* Killer)
{

}

void ASpawnManager::StartRespawnPlayer(APlayerController* Controller)
{
	if (Controller)
	{
		FTimerHandle RespawnHandle;
		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindUFunction(this, FName("SpawnPlayer"), Controller);
		GetWorld()->GetTimerManager().SetTimer(RespawnHandle, RespawnDelegate, GM->MinRespawnDelay, false);
	}
}

void ASpawnManager::SpawnPlayer(APlayerController* Controller)
{
	if (Controller)
	{
		GM->RestartPlayer(Controller);
	}
}
