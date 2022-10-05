// Fill out your copyright notice in the Description page of Project Settings.

#include "BottleCapGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"
#include "BottleCapRaceGame/Helpers/FormatMessage.h"

/**
 * @brief Construct a new UBottleCapGameInstance::UBottleCapGameInstance object
 * 
 */
UBottleCapGameInstance::UBottleCapGameInstance()
{
}

/**
 * @brief Ajusta os controles para o jogo.
 * 
 */
void UBottleCapGameInstance::SetupInputControllForGame()
{
	if (UWorld *World = GetWorld())
	{
		APlayerController *PlayerController = World->GetFirstPlayerController();

		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG();
#endif
}

/**
 * @brief Executa quando o mapa é completamente carregado.
 * 
 * @param LoadTime 
 * @param MapName 
 */
void UBottleCapGameInstance::LoadComplete(const float LoadTime, const FString &MapName)
{
	Super::LoadComplete(LoadTime, MapName);

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG_2(FString::SanitizeFloat(LoadTime), *MapName);
#endif

	if (MapName.Equals("/Game/Maps/INTROD"))
	{
		SetupInputControllForUI();
	}
	else if (MapName.Equals("/Game/Maps/LOBBY"))
	{
		SetupInputControllForGame();
	}
}

/**
 * @brief Ajusta os controles para usar o menu.
 * 
 */
void UBottleCapGameInstance::SetupInputControllForUI()
{
	if (UWorld *World = GetWorld())
	{

		APlayerController *PlayerController = World->GetFirstPlayerController();

		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG();
#endif
}
