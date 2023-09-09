// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "GameFramework/Character.h"

#include "HideAndKillGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerDiedSignature, ACharacter*, Player, ACharacter*, Killer);

/**
 * 
 */
UCLASS()
class HIDEANDKILL_API AHideAndKillGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AHideAndKillGameMode();

	/* Multicast delegate for when a player died */
	UPROPERTY(BlueprintAssignable)
	FPlayerDiedSignature PlayerDiedDelegate;

	// Called when a player character had been killed
	UFUNCTION()
	void OnPlayerKilled(APlayerController* Player, APlayerController* Killer);
	// Called when a NPC had been killed
	UFUNCTION()
	void OnNPCKilled(APlayerController* Killer);

private:
	// Start delay to respawn a player
	void StartRespawnPlayer(APlayerController* Controller);
	// Spawn player at a random spawn point
	UFUNCTION()
	void SpawnPlayer(APlayerController* Controller);
	// Players should not respawn at their starting spawn point
	bool ShouldSpawnAtStartSpot(AController* Player) override { return false; };

	FTimerHandle RespawnHandle;
};
