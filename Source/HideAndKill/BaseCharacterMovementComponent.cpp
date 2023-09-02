// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterMovementComponent.h"

#include <GameFramework/Character.h>

UBaseCharacterMovementComponent::UBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
    Sprint_MaxWalkSpeed = 500.f;
    MaxWalkSpeed = 200.f;
}

void UBaseCharacterMovementComponent::SetSprinting(bool bSprinting)
{
    bWantsToSprint = bSprinting;
}

void UBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
    Super::UpdateFromCompressedFlags(Flags);
    bWantsToSprint = ((Flags & FSavedMove_BaseCharacter::FLAG_WantsToCrouch) != 0);
}

FNetworkPredictionData_Client* UBaseCharacterMovementComponent::GetPredictionData_Client() const
{
    if (ClientPredictionData == nullptr)
    {
        UBaseCharacterMovementComponent* MutableThis = const_cast<UBaseCharacterMovementComponent*>(this);
        MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_BaseCharacter(*this);
    }

    return ClientPredictionData;
}

float UBaseCharacterMovementComponent::GetMaxSpeed() const
{
    if (bWantsToSprint && IsMovingOnGround())
    {
        return Sprint_MaxWalkSpeed;
    }
    return Super::GetMaxSpeed();
    
}

bool UBaseCharacterMovementComponent::FSavedMove_BaseCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
    FSavedMove_BaseCharacter* NewBaseCharacterMove = static_cast<FSavedMove_BaseCharacter*>(NewMove.Get());
    if (bWantsToSprint != NewBaseCharacterMove->bWantsToSprint)
    {
        return false;
    }

    return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UBaseCharacterMovementComponent::FSavedMove_BaseCharacter::Clear()
{
    Super::Clear();
    bWantsToSprint = false;
}

uint8 UBaseCharacterMovementComponent::FSavedMove_BaseCharacter::GetCompressedFlags() const
{
    uint8 Result = Super::GetCompressedFlags();

    if (bWantsToSprint)
    {
        Result |= FLAG_Custom_0;
    }

    return Result;
}

void UBaseCharacterMovementComponent::FSavedMove_BaseCharacter::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
    Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);
    UBaseCharacterMovementComponent* CharacterMovement = Cast<UBaseCharacterMovementComponent>(Character->GetCharacterMovement());
    bWantsToSprint = CharacterMovement->bWantsToSprint;
}

UBaseCharacterMovementComponent::FNetworkPredictionData_Client_BaseCharacter::FNetworkPredictionData_Client_BaseCharacter(const UCharacterMovementComponent& ClientMovement)
: Super(ClientMovement)
{
}

FSavedMovePtr UBaseCharacterMovementComponent::FNetworkPredictionData_Client_BaseCharacter::AllocateNewMove()
{
    return FSavedMovePtr(new FSavedMove_BaseCharacter());
}
