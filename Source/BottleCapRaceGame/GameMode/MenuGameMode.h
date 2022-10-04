// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BOTTLECAPRACEGAME_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMenuGameMode();

	virtual void PostLogin(APlayerController *NewPlayer) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void StartPlay();
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot);
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
