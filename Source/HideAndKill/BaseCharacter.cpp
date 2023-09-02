// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "BaseCharacterMovementComponent.h"

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
	Destroy();
}
