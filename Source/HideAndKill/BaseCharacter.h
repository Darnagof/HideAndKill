// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Killable.h"
#include "BaseCharacter.generated.h"

class UBaseCharacterMovementComponent;

/**
* Base character class inherited by Player and NPC
*/
UCLASS()
class HIDEANDKILL_API ABaseCharacter : public ACharacter, public IKillable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable)
	void SetSprinting(bool bEnable);

	// Replacement for GetCharacterMovement, because can't Cast<> on FORCEINLINE method
	UFUNCTION(BlueprintCallable)
	UBaseCharacterMovementComponent* GetBaseCharacterMovement() const;

	// Killable interface

	virtual void OnKill(AActor* Killer) override;
};
