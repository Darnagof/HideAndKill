// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include <GameFramework/CharacterMovementComponent.h>

const float ABaseCharacter::WalkSpeed = 200.f;
const float ABaseCharacter::SprintSpeed = 500.f;

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::Sprint(bool bEnable)
{
	GetCharacterMovement()->MaxWalkSpeed = bEnable ? SprintSpeed : WalkSpeed;
}
