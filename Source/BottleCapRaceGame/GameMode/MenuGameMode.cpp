// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuGameMode.h"

#include "BottleCapRaceGame/Pawns/MenuPlayerPawn.h"
#include "BottleCapRaceGame/Pawns/BottleCapPlayerState.h"
#include "BottleCapRaceGame/BottleCapGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "BottleCapRaceGame/Helpers/FormatMessage.h"

/**
 * @brief Construct a new AMenuGameMode::AMenuGameMode object
 * 
 */
AMenuGameMode::AMenuGameMode()
{
#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif
    DefaultPawnClass = AMenuPlayerPawn::StaticClass();
    // PlayerStateClass = ABottleCapPlayerState::StaticClass();
    // GameStateClass = ABottleCapGameState::StaticClass();

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG_1(DefaultPawnClass.Get()->GetFullName());
#endif
}

/**
 * @brief Método executado depois do login ser efetuado no mapa.
 * 
 * @param NewPlayer 
 */
void AMenuGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif

}


/**
 * @brief Sobrecarga que executa antes do player ser "spawned" no mapa.
 * 
 * @param NewPlayer 
 */
void AMenuGameMode::HandleStartingNewPlayer_Implementation(APlayerController *NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif
}

/**
 * @brief Método para iniciar o jogo.
 * 
 */
void AMenuGameMode::StartPlay()
{
    Super::StartPlay();

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif
}

/**
 * @brief Reinicia o player em um local especifico.
 * 
 * @param NewPlayer 
 * @param StartSpot 
 */
void AMenuGameMode::RestartPlayerAtPlayerStart(AController *NewPlayer, AActor *StartSpot)
{
    Super::RestartPlayerAtPlayerStart(NewPlayer, StartSpot);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG_1(StartSpot->GetActorLocation().ToString());
#endif
}


/**
 * @brief Sobrecarga do método de escolha do local de spawn do player.
 * 
 * @param Player 
 * @return AActor* 
 */
AActor *AMenuGameMode::ChoosePlayerStart_Implementation(AController *Player)
{
    Super::ChoosePlayerStart_Implementation(Player);

#if UE_BUILD_DEVELOPMENT
    PRINT_LOG();
#endif

    TArray<AActor *> OutPlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutPlayerStarts);

    if (OutPlayerStarts.Num() > 0)
    {
        return OutPlayerStarts[0];
    }

    return Super::ChoosePlayerStart_Implementation(Player);
}