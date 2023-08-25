// Fill out your copyright notice in the Description page of Project Settings.

#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "HideAndKillGameMode.h"

AHideAndKillGameMode::AHideAndKillGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
