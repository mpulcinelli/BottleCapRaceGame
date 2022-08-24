// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BottleCapRaceGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BOTTLECAPRACEGAME_API ABottleCapRaceGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	ABottleCapRaceGameGameModeBase();

	protected:
	
	virtual void StartPlay() override;
	//virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	//virtual void RestartPlayer(AController* NewPlayer) override;
};
