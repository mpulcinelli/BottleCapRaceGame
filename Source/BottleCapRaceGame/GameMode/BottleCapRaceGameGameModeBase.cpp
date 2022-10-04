// Copyright Epic Games, Inc. All Rights Reserved.

#include "BottleCapRaceGameGameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerPawn.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerState.h"
#include "BottleCapRaceGame/BottleCapGameState.h"
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
}

void ABottleCapRaceGameGameModeBase::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();
    PRINT_LOG();

    GetGameState<ABottleCapGameState>()->CurrentPlayer = GameState->PlayerArray[IndexPlayer];
}

int32 ABottleCapRaceGameGameModeBase::GetPlayerIdToPlay() const
{
    return GameState->PlayerArray[IndexPlayer]->GetPlayerId();
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
        // int32 NextPlayer = GetGameState<ABottleCapGameState>()->PlayerTurn;
        GetGameState<ABottleCapGameState>()->NumOfFlicks = 3;
        int NumTotalPlayers = GameState->PlayerArray.Num();
        IndexPlayer++;

        if (IndexPlayer >= NumTotalPlayers)
        {
            IndexPlayer = 0;
        }

        GetGameState<ABottleCapGameState>()->CurrentPlayer = GameState->PlayerArray[IndexPlayer];

        OnChangePlayerToPlay.Broadcast(GetPlayerIdToPlay());
        OnChangeRemainingMoves.Broadcast(GetPlayerRemainingMoves());
    }
}

void ABottleCapRaceGameGameModeBase::GoNextPlayer()
{
    int NumTotalPlayers = GameState->PlayerArray.Num();
    IndexPlayer++;

    if (IndexPlayer >= NumTotalPlayers)
    {
        IndexPlayer = 0;
    }

    GetGameState<ABottleCapGameState>()->NumOfFlicks = 3;

    OnChangePlayerToPlay.Broadcast(GetPlayerIdToPlay());
    OnChangeRemainingMoves.Broadcast(GetPlayerRemainingMoves());
}

AActor *ABottleCapRaceGameGameModeBase::ChoosePlayerStart_Implementation(AController *Player)
{
    AActor *PlayStartFromSup = Super::ChoosePlayerStart_Implementation(Player);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif

    TArray<AActor *> OutPlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutPlayerStarts);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG_1(FString::FromInt(CountPlayers));
#endif

    switch (CountPlayers)
    {
    case 0:
    {
        CountPlayers++;
        return OutPlayerStarts[0];
    }
    break;
    case 1:
    {
        CountPlayers++;
        return OutPlayerStarts[1];
    }
    break;
    case 2:
    {
        CountPlayers++;
        return OutPlayerStarts[2];
    }
    break;
    case 3:
    {
        CountPlayers++;
        return OutPlayerStarts[3];
    }
    break;

    default:
        return PlayStartFromSup;
        break;
    }
}

void ABottleCapRaceGameGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController *NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif

    ABottleCapPlayerPawn *MyPawn = NewPlayer->GetPawn<ABottleCapPlayerPawn>();
    if (!MyPawn)
    {
#if UE_BUILD_DEVELOPMENT
        PRINT_LOG_1("NO PAWN { ABottleCapPlayerPawn }");
#endif

        return;
    }

    TArray<AActor *> OutPlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutPlayerStarts);

    int32 NumberOfPlayers = GameState->PlayerArray.Num();

    int32 PlayerId = MyPawn->GetPlayerState<ABottleCapPlayerState>()->GetPlayerId();

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG_2("My Pawn::PlayerId", FString::FromInt(PlayerId));
#endif

    MyPawn->UpdateMyName(PlayerId);

    OnChangePlayerToPlay.Broadcast(GetPlayerIdToPlay());
    OnChangeRemainingMoves.Broadcast(GetPlayerRemainingMoves());

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG_2("NumberOfPlayers", FString::FromInt(NumberOfPlayers));
#endif
}
