// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

void ANPC_AIController::BeginPlay()
{
	Super::BeginPlay();
}

void ANPC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// Init Blackboard and launch Behavior Tree
	if (IsValid(BehaviorTree.Get()))
	{
		UBlackboardComponent *b;
		UseBlackboard(BehaviorTree->BlackboardAsset, b);
		RunBehaviorTree(BehaviorTree);
	}
}
