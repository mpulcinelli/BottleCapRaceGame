// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerPawn.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerState.h"
#include "BottleCapRaceGame/BottleCapGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "BottleCapRaceGame/Helpers/FormatMessage.h"

ALobbyGameMode::ALobbyGameMode()
{
    DefaultPawnClass = ABottleCapPlayerPawn::StaticClass();
    PlayerStateClass = ABottleCapPlayerState::StaticClass();
    GameStateClass = ABottleCapGameState::StaticClass();
}

void ALobbyGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    PRINT_LOG();

    
    if (HasAuthority())
    {

        int32 NumberOfPlayers = GameState->PlayerArray.Num();

        if (NumberOfPlayers == 2)
        {

            
            for (auto &&i : GameState->PlayerArray)
            {
                PRINT_LOG_2("PlayerId", FString::FromInt(i->GetPlayerId()));
            }
            

            UWorld *World = GetWorld();
            if (World)
            {
                bUseSeamlessTravel = true;
                World->ServerTravel("/Game/Maps/RaceMap?listen");
            }
        }
        else
        {
            ABottleCapPlayerPawn *MyPawn = NewPlayer->GetPawn<ABottleCapPlayerPawn>();

            if (!MyPawn)
            {
                PRINT_LOG_1("NO PAWN { ABottleCapPlayerPawn }");
                return;
            }

            TArray<AActor *> OutPlayerStarts;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutPlayerStarts);

            switch (NumberOfPlayers - 1)
            {
            case 0:
            {
                const FVector AcLocation = OutPlayerStarts[0]->GetActorLocation();
                MyPawn->SetActorLocation(AcLocation);
            }
            break;
            case 1:
            {
                const FVector AcLocation = OutPlayerStarts[1]->GetActorLocation();
                MyPawn->SetActorLocation(AcLocation);
            }
            break;
            case 2:
            {
                const FVector AcLocation = OutPlayerStarts[2]->GetActorLocation();
                MyPawn->SetActorLocation(AcLocation);
            }
            break;
            case 3:
            {
                const FVector AcLocation = OutPlayerStarts[3]->GetActorLocation();
                MyPawn->SetActorLocation(AcLocation);
            }
            break;

            default:
                break;
            }
        }
    }
}

void ALobbyGameMode::HandleStartingNewPlayer_Implementation(APlayerController *NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    PRINT_LOG();

}