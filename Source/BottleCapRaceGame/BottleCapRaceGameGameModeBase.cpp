// Copyright Epic Games, Inc. All Rights Reserved.


#include "BottleCapRaceGameGameModeBase.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerPawn.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerState.h"

ABottleCapRaceGameGameModeBase::ABottleCapRaceGameGameModeBase()
{
    DefaultPawnClass = ABottleCapPlayerPawn::StaticClass();
    PlayerStateClass = ABottleCapPlayerState::StaticClass();
}

void ABottleCapRaceGameGameModeBase::StartPlay()
{
    Super::StartPlay();
    UE_LOG(LogTemp, Warning, TEXT("StartPlay"));
}

// void ABottleCapRaceGameGameModeBase::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
// {
//     Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
//     auto NewStart = FindPlayerStart(NewPlayer, "P2");
//     StartSpot = NewStart;
    
//     UE_LOG(LogTemp, Warning, TEXT("RestartPlayerAtPlayerStart"));
// }

// void ABottleCapRaceGameGameModeBase::RestartPlayer(AController* NewPlayer)
// {
//     Super::RestartPlayer(NewPlayer);
//     UE_LOG(LogTemp, Warning, TEXT("RestartPlayer"));
// }
