// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GameFramework/PlayerStart.h"
#include "HideAndKill/NPC.h"
#include "SpawnManager.generated.h"

class AHideAndKillGameMode;

/**
 * Manage spawn of players and NPC
 * This Actor will interface itself with the HideAndKillGameMode to call respawn functions and receive Players/NPC killed events
 * (only used by Server)
 */
UCLASS()
class HIDEANDKILL_API ASpawnManager : public AInfo
{
	GENERATED_BODY()

public:
	ASpawnManager();
	void BeginPlay() override;
	// Called by GameMode
	void InitManager();

	// List of active spawn points for players and NPC
	UPROPERTY(EditInstanceOnly)
	TSet<APlayerStart*> SpawnPoints;

	// Start delay to respawn a player
	void StartRespawnPlayer(APlayerController* Controller);
	// Ask GameMode to spawn player
	UFUNCTION()
	void SpawnPlayer(APlayerController* Controller);
	// Respawn NPC
	UFUNCTION()
	void OnNPCKilled(AAIController* NPC, APlayerController* Killer);
	// Assign a pawn to all AI controllers and spawn them on the level
	void SpawnAllNPC();

private:
	// Reference to H&K GameMode
	AHideAndKillGameMode* GM = nullptr;
	// Max number of NPC that can be active on the level
	UPROPERTY(EditInstanceOnly, DisplayName = "Max NPC population")
	uint16 MaxNPCPopulation = 0;
	// AI Controllers
	TArray<TWeakObjectPtr<class AAIController> > AIControllers; // TODO move this to a future AI Manager ? (+ use NPCController instead ?)

	UFUNCTION()
	void OnPlayerKilled(APlayerController* Player, APlayerController* Killer);
};
