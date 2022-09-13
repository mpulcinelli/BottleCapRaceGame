// Copyright Epic Games, Inc. All Rights Reserved.

#include "BottleCapRaceGameGameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerPawn.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerState.h"
#include "BottleCapRaceGame/BottleCapGameState.h"
#include "BottleCapRaceGame/Controller/BottleCapPlayerController.h"
#include "BottleCapRaceGame/BottleCapGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "BottleCapRaceGame/Helpers/FormatMessage.h"

ABottleCapRaceGameGameModeBase::ABottleCapRaceGameGameModeBase()
{
    DefaultPawnClass = ABottleCapPlayerPawn::StaticClass();
    PlayerStateClass = ABottleCapPlayerState::StaticClass();
    GameStateClass = ABottleCapGameState::StaticClass();
}

void ABottleCapRaceGameGameModeBase::StartPlay()
{
    Super::StartPlay();
    PRINT_LOG();

    TArray<int32> players;

    for (auto &&i :  GameState->PlayerArray)
    {
        players.Add(i->GetPlayerId());
    }

    players.Sort();

    //GetGameState<ABottleCapGameState>()->PlayerTurn = GameState->PlayerArray[0]->GetPlayerId();
    GetGameState<ABottleCapGameState>()->PlayerTurn = players[0];
    PRINT_LOG_2("PlayerArray[0]::PlayerId", FString::FromInt(GameState->PlayerArray[0]->GetPlayerId()));
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

        int NumTotalPlayers = GameState->PlayerArray[0]->GetPlayerId()+GetNumPlayers()-1;

        if (NextPlayer >= NumTotalPlayers)
        {
            GetGameState<ABottleCapGameState>()->PlayerTurn = GameState->PlayerArray[0]->GetPlayerId();
        }
        else
        {
            GetGameState<ABottleCapGameState>()->PlayerTurn = NextPlayer + 1;
        }

        OnChangePlayerToPlay.Broadcast(GetPlayerIdToPlay());
        OnChangeRemainingMoves.Broadcast(GetPlayerRemainingMoves());
    }
}

void ABottleCapRaceGameGameModeBase::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);
    PRINT_LOG();
}

void ABottleCapRaceGameGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController *NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    PRINT_LOG();
  
    if (HasAuthority())
    {
        ABottleCapPlayerPawn *MyPawn = NewPlayer->GetPawn<ABottleCapPlayerPawn>();
        if (!MyPawn)
        {
            PRINT_LOG_1("NO PAWN { ABottleCapPlayerPawn }");
            return;
        }

        TArray<AActor *> OutPlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutPlayerStarts);

        int32 NumberOfPlayers = GameState->PlayerArray.Num();

        int32 PlayerId = MyPawn->GetPlayerState<ABottleCapPlayerState>()->GetPlayerId();

        PRINT_LOG_2("My Pawn::PlayerId", FString::FromInt(PlayerId));

        MyPawn->UpdateMyName(PlayerId);

        switch (NumberOfPlayers - 1)
        {
        case 0:
        {
            RestartPlayerAtPlayerStart(MyPawn->GetController(), OutPlayerStarts[0]);
        }
        break;
        case 1:
        {
            RestartPlayerAtPlayerStart(MyPawn->GetController(), OutPlayerStarts[1]);
        }
        break;
        case 2:
        {
            RestartPlayerAtPlayerStart(MyPawn->GetController(), OutPlayerStarts[2]);
        }
        break;
        case 3:
        {
            RestartPlayerAtPlayerStart(MyPawn->GetController(), OutPlayerStarts[3]);
        }
        break;

        default:
            break;
        }

        OnChangePlayerToPlay.Broadcast(GetPlayerIdToPlay());
        OnChangeRemainingMoves.Broadcast(GetPlayerRemainingMoves());

        PRINT_LOG_2("NumberOfPlayers", FString::FromInt(NumberOfPlayers));
    }
}
