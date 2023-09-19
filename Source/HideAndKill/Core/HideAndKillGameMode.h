// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "GameFramework/Character.h"
#include "SpawnManager.h"

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
	virtual void PostInitializeComponents() override;

	/** The default pawn class used by AI. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
	TSubclassOf<APawn> DefaultAIPawnClass;

	/* Multicast delegate for when a player died */
	UPROPERTY(BlueprintAssignable)
	FPlayerDiedSignature PlayerDiedDelegate;

	// Called when a player character had been killed
	UFUNCTION()
	void OnPlayerKilled(APlayerController* Player, APlayerController* Killer);
	// Called when a NPC had been killed
	UFUNCTION()
	void OnNPCKilled(AAIController* NPC, APlayerController* Killer);
	// Returns default Pawn class for Player or AI
	UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	// Choose a spawn point from SpawnManager list
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	// Manages spawns and NPC population
	ASpawnManager* SpawnManager;
	// Players should not respawn at their starting spawn point
	bool ShouldSpawnAtStartSpot(AController* Player) override { return false; };
};
