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

/**
 * @brief Construct a new ALobbyGameMode::ALobbyGameMode object
 * 
 */
ALobbyGameMode::ALobbyGameMode()
{
    DefaultPawnClass = ABottleCapPlayerPawn::StaticClass();
    PlayerStateClass = ABottleCapPlayerState::StaticClass();
    GameStateClass = ABottleCapGameState::StaticClass();
}

/**
 * @brief Método para iniciar o jogo.
 * 
 */
void ALobbyGameMode::StartPlay()
{
    Super::StartPlay();
#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif
}

/**
 * @brief Método executado depois do login ser efetuado no mapa.
 * 
 * @param NewPlayer 
 */
void ALobbyGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif

    if (CountPlayers == 2 )
    {
        UWorld *World = GetWorld();
        if (World)
        {
            bUseSeamlessTravel = true;
            World->ServerTravel("/Game/Maps/RaceMap?listen");
        }
    }
}

/**
 * @brief Sobrecarga do método de escolha do local de spawn do player.
 * 
 * @param Player 
 * @return AActor* 
 */
AActor *ALobbyGameMode::ChoosePlayerStart_Implementation(AController *Player)
{
    AActor *PlayStartFromSup = Super::ChoosePlayerStart_Implementation(Player);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif

    TArray<AActor *> OutPlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutPlayerStarts);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG_2("CountPlayers", FString::FromInt(CountPlayers));
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