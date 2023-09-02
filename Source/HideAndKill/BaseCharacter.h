// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UBaseCharacterMovementComponent;

UCLASS()
class HIDEANDKILL_API ABaseCharacter : public ACharacter
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
};
