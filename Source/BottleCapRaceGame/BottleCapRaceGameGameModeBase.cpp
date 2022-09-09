// Copyright Epic Games, Inc. All Rights Reserved.

#include "BottleCapRaceGameGameModeBase.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerPawn.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerState.h"
#include "BottleCapRaceGame/BottleCapGameState.h"
#include "BottleCapRaceGame/Controller/BottleCapPlayerController.h"
#include "BottleCapRaceGame/BottleCapGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerStart.h"

ABottleCapRaceGameGameModeBase::ABottleCapRaceGameGameModeBase()
{
    DefaultPawnClass = ABottleCapPlayerPawn::StaticClass();
    PlayerStateClass = ABottleCapPlayerState::StaticClass();
    GameStateClass = ABottleCapGameState::StaticClass();
    // PlayerControllerClass = ABottleCapPlayerController::StaticClass();
}

void ABottleCapRaceGameGameModeBase::StartPlay()
{
    Super::StartPlay();
    UE_LOG(LogTemp, Warning, TEXT("ABottleCapRaceGameGameModeBase::StartPlay"));

    if (HasAuthority())
    {
        auto GI = GetGameInstance<UBottleCapGameInstance>();
        if (GI)
        {
            UE_LOG(LogTemp, Warning, TEXT("StartPlay -- StartOnlineGame"));
        }
    }
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

void ABottleCapRaceGameGameModeBase::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    OnChangePlayerToPlay.Broadcast(GetPlayerIdToPlay());
    OnChangeRemainingMoves.Broadcast(GetPlayerRemainingMoves());

    UE_LOG(LogTemp, Warning, TEXT("ABottleCapRaceGameGameModeBase::PostLogin"));
}

void ABottleCapRaceGameGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController *NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    ABottleCapPlayerPawn *MyPawn = NewPlayer->GetPawn<ABottleCapPlayerPawn>();

    if (!MyPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("ABottleCapRaceGameGameModeBase::HandleStartingNewPlayer_Implementation NO PAWN!!!!!!!!!!!!!!!!"));
        return;
    }

    TArray<AActor *> OutPawn;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), OutPawn);

    int32 QtdActors = OutPawn.Num();

    UE_LOG(LogTemp, Warning, TEXT("ABottleCapRaceGameGameModeBase::HandleStartingNewPlayer_Implementation PAWNS %d"), QtdActors);

    TArray<AActor *> OutPlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutPlayerStarts);
    
    FVector AcLocation;

    switch (QtdActors)
    {
    case 0:
        AcLocation = OutPlayerStarts[0]->GetActorLocation();
        MyPawn->SetActorLocation(AcLocation);
        break;
    case 1:
        AcLocation = OutPlayerStarts[1]->GetActorLocation();
        MyPawn->SetActorLocation(AcLocation);
        break;
    case 2:
        AcLocation = OutPlayerStarts[2]->GetActorLocation();
        MyPawn->SetActorLocation(AcLocation);
        break;
    case 3:
        AcLocation = OutPlayerStarts[3]->GetActorLocation();
        MyPawn->SetActorLocation(AcLocation);
        break;

    default:
        break;
    }

    UE_LOG(LogTemp, Warning, TEXT("ABottleCapRaceGameGameModeBase::HandleStartingNewPlayer_Implementation"));
}

void ABottleCapRaceGameGameModeBase::GameWelcomePlayer(UNetConnection *Connection, FString &RedirectURL)
{

    Super::GameWelcomePlayer(Connection, RedirectURL);

    UE_LOG(LogTemp, Warning, TEXT("ABottleCapRaceGameGameModeBase::GameWelcomePlayer"));
}