// Fill out your copyright notice in the Description page of Project Settings.

#include "HideAndKillGameMode.h"
#include "HideAndKill/TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"

AHideAndKillGameMode::AHideAndKillGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	// On player died
	//PlayerDiedDelegate.AddUniqueDynamic(this, &AHideAndKillGameMode::OnPlayerDied);
}

void AHideAndKillGameMode::OnPlayerKilled(APlayerController* Player, APlayerController* Killer)
{
	if (Killer == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Player %s died"), *Player->GetPlayerState<APlayerState>()->GetPlayerName());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Player %s killed %s"), *Player->GetPlayerState<APlayerState>()->GetPlayerName(), *Killer->GetPlayerState<APlayerState>()->GetPlayerName());
	}
	StartRespawnPlayer(Player);
}

void AHideAndKillGameMode::OnNPCKilled(APlayerController* Killer)
{
}

void AHideAndKillGameMode::StartRespawnPlayer(APlayerController* Controller)
{
	if (Controller)
	{
		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindUFunction(this, FName("SpawnPlayer"), Controller);
		GetWorld()->GetTimerManager().SetTimer(RespawnHandle, RespawnDelegate, MinRespawnDelay, false);
	}
}

void AHideAndKillGameMode::SpawnPlayer(APlayerController* Controller)
{
	if (Controller)
	{
		RestartPlayer(Controller);
	}
}
