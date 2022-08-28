// Copyright Epic Games, Inc. All Rights Reserved.

#include "BottleCapRaceGameGameModeBase.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerPawn.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerState.h"
#include "BottleCapRaceGame/BottleCapGameState.h"
#include "BottleCapRaceGame/Controller/BottleCapPlayerController.h"

ABottleCapRaceGameGameModeBase::ABottleCapRaceGameGameModeBase()
{
    DefaultPawnClass = ABottleCapPlayerPawn::StaticClass();
    PlayerStateClass = ABottleCapPlayerState::StaticClass();
    GameStateClass = ABottleCapGameState::StaticClass();
    //PlayerControllerClass = ABottleCapPlayerController::StaticClass();
}

void ABottleCapRaceGameGameModeBase::StartPlay()
{
    Super::StartPlay();
    UE_LOG(LogTemp, Warning, TEXT("StartPlay"));
}

int32 ABottleCapRaceGameGameModeBase::GetPlayerIdToPlay() const
{
    return GetGameState<ABottleCapGameState>()->PlayerTurn;
}

int32 ABottleCapRaceGameGameModeBase::GetPlayerRemainingMoves() const
{
    return GetGameState<ABottleCapGameState>()->NumOfFlicks;
}

void ABottleCapRaceGameGameModeBase::MoveCap()
{
    int32 Num = GetGameState<ABottleCapGameState>()->NumOfFlicks;

    if (Num > 1)
    {
        GetGameState<ABottleCapGameState>()->NumOfFlicks = Num - 1;
        OnChangeRemainingMoves.Broadcast(GetPlayerRemainingMoves());
    }
    else
    {
        int32 NextPlayer = GetGameState<ABottleCapGameState>()->PlayerTurn;
        GetGameState<ABottleCapGameState>()->NumOfFlicks = 3;

        int NumTotalPlayers = GetNumPlayers();

        if (NextPlayer >= NumTotalPlayers)
        {
            GetGameState<ABottleCapGameState>()->PlayerTurn = 1;
        }
        else
        {
            GetGameState<ABottleCapGameState>()->PlayerTurn = NextPlayer + 1;
        }

        OnChangePlayerToPlay.Broadcast(GetPlayerIdToPlay());
        OnChangeRemainingMoves.Broadcast(GetPlayerRemainingMoves());
    }
}

void ABottleCapRaceGameGameModeBase::GameWelcomePlayer(UNetConnection *Connection, FString &RedirectURL){
    
    Super::GameWelcomePlayer(Connection, RedirectURL);

    UE_LOG(LogTemp, Warning, TEXT("GameWelcomePlayer"));
    
    OnChangePlayerToPlay.Broadcast(GetPlayerIdToPlay());
    OnChangeRemainingMoves.Broadcast(GetPlayerRemainingMoves());

}