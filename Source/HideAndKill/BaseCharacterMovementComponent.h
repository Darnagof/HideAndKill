// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterMovementComponent.generated.h"

/**
 * Inherited CharacterMovementComponent for additional movement mechanics (Sprint)
 */
UCLASS()
class HIDEANDKILL_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_BaseCharacter : public FSavedMove_Character
	{
	public:
		typedef FSavedMove_Character Super;

		static const int FLAG_WantsToCrouch = FLAG_Custom_0;
		uint32 bWantsToSprint:1;

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	};

	class FNetworkPredictionData_Client_BaseCharacter : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_BaseCharacter(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly)
	float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly)
	float Walk_MaxWalkSpeed;

	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToSprint:1;

	UFUNCTION(BlueprintCallable)
	void SetSprinting(bool bSprinting);

	virtual float GetMaxSpeed() const override;
	void UpdateFromCompressedFlags(uint8 Flags) override;
	FNetworkPredictionData_Client* GetPredictionData_Client() const override;
};
