// Fill out your copyright notice in the Description page of Project Settings.

#include "HideAndKillGameMode.h"
#include "HideAndKill/TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "HideAndKill/NPC.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnManager.h"
#include "TimerManager.h"
#include "AIController.h"
#include "Engine/PlayerStartPIE.h"

AHideAndKillGameMode::AHideAndKillGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<APawn> AIPawnBPClass(TEXT("/Game/AI/BP_NPC"));
	if (AIPawnBPClass.Class != NULL)
	{
		DefaultAIPawnClass = AIPawnBPClass.Class;
	}
	// On player died
	//PlayerDiedDelegate.AddUniqueDynamic(this, &AHideAndKillGameMode::OnPlayerDied);
}

void AHideAndKillGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// SpawnManager is initialized here to ensure it's available before any client login

	// Find SpawnManager, create it if not found
	AActor* FoundSpawnManager = UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass());
	if (!FoundSpawnManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnManager missing, creating a default one"));
		SpawnManager = GetWorld()->SpawnActor<ASpawnManager>();
	}
	else
	{
		SpawnManager = Cast<ASpawnManager>(FoundSpawnManager);
	}
	// Init spawn manager
	SpawnManager->InitManager();
}

void AHideAndKillGameMode::OnPlayerKilled(APlayerController* Player, APlayerController* Killer)
{
	if (Killer == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Player %s died"), *Player->GetPlayerState<APlayerState>()->GetPlayerName());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Player %s killed %s"), *Player->GetPlayerState<APlayerState>()->GetPlayerName(), *Killer->GetPlayerState<APlayerState>()->GetPlayerName());
	}
	SpawnManager->StartRespawnPlayer(Player);
}

void AHideAndKillGameMode::OnNPCKilled(AAIController* NPC, APlayerController* Killer)
{
	SpawnManager->OnNPCKilled(NPC, Killer);
}

UClass* AHideAndKillGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (Cast<AAIController>(InController))
	{
		return DefaultAIPawnClass;
	}
	return DefaultPawnClass;
}

AActor* AHideAndKillGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// Choose a player start
	APlayerStart* FoundPlayerStart = nullptr;
	UClass* PawnClass = GetDefaultPawnClassForController(Player);
	APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
	TArray<APlayerStart*> UnOccupiedStartPoints;
	TArray<APlayerStart*> OccupiedStartPoints;
	UWorld* World = GetWorld();
	// Choose an available random spawn point from SpawnManager list
	for (auto It = SpawnManager->SpawnPoints.CreateConstIterator(); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (PlayerStart->IsA<APlayerStartPIE>())
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			FoundPlayerStart = PlayerStart;
			break;
		}
		else
		{
			FVector ActorLocation = PlayerStart->GetActorLocation();
			const FRotator ActorRotation = PlayerStart->GetActorRotation();
			if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
			{
				UnOccupiedStartPoints.Add(PlayerStart);
			}
			else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
			{
				OccupiedStartPoints.Add(PlayerStart);
			}
		}
	}
	if (FoundPlayerStart == nullptr)
	{
		if (UnOccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
		}
		else if (OccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
		}
	}
	return FoundPlayerStart;
}
