// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Killable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UKillable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HIDEANDKILL_API IKillable
{
	GENERATED_BODY()

public:
	virtual void OnKill(AActor* Killer) = 0;
};
