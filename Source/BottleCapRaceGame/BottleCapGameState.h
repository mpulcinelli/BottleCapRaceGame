// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BottleCapGameState.generated.h"

/**
 *
 */
UCLASS()
class BOTTLECAPRACEGAME_API ABottleCapGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ABottleCapGameState();
	~ABottleCapGameState() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APlayerState *CurrentPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumOfFlicks;
};
