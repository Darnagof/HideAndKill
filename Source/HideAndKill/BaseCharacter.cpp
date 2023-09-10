// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "BaseCharacterMovementComponent.h"
#include "Core/HideAndKillGameMode.h"

// Sets default values
ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
}

void ABaseCharacter::SetSprinting(bool bEnable)
{
	if (auto mc = GetBaseCharacterMovement())
	{
		mc->bWantsToSprint = bEnable;
	}
}

UBaseCharacterMovementComponent* ABaseCharacter::GetBaseCharacterMovement() const
{
	return StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());
}

void ABaseCharacter::OnKill(AActor* Killer)
{
	UE_LOG(LogTemp, Log, TEXT("Killed by %s"), *Killer->GetName());
	
	if (HasAuthority()) // TODO use event instead ?
	{
		NotifyGameModeOnKill(Killer);
	}
	K2_OnKill(Killer);
}

void ABaseCharacter::K2_OnKill_Implementation(AActor* Killer)
{
	Destroy(); // TODO safe to call Destroy after requesting respawn ?
}

void ABaseCharacter::NotifyGameModeOnKill(AActor* Killer)
{
	AHideAndKillGameMode * GM = GetWorld()->GetAuthGameMode<AHideAndKillGameMode>();
	if (!GM) return;

	APlayerController* KillerController = nullptr;
	if (APawn* KillerPawn = Cast<APawn>(Killer))
	{
		KillerController = Cast<APlayerController>(KillerPawn->GetController());
	}

	// If killed target is a player
	if (IsPlayerControlled())
	{
		GM->OnPlayerKilled(Cast<APlayerController>(GetController()), KillerController);
	}
	// If killed target is NPC
	else
	{
		GM->OnNPCKilled(KillerController);
	}
}
