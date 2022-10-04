// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BottleCapRaceGameGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerToPlaySignature, int32, PlayerToPlay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRemainingMovesSignature, int32, RemainingMoves);

/**
 *
 */
UCLASS()
class BOTTLECAPRACEGAME_API ABottleCapRaceGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABottleCapRaceGameGameModeBase();
	virtual ~ABottleCapRaceGameGameModeBase() = default;

	UPROPERTY(BlueprintAssignable)
	FPlayerToPlaySignature OnChangePlayerToPlay;

	UPROPERTY(BlueprintAssignable)
	FRemainingMovesSignature OnChangeRemainingMoves;


	/**
	 * Recupera o player que pode jogar.
	 */
	UFUNCTION(BlueprintPure, Category = "Game Strategy")
	virtual int32 GetPlayerIdToPlay() const;

	UFUNCTION(BlueprintPure, Category = "Game Strategy")
	virtual int32 GetPlayerRemainingMoves() const;

	UFUNCTION(Category = "Game Strategy")
	virtual void MoveCap();

	UFUNCTION(Category = "Game Strategy")
	virtual void GoNextPlayer();

protected:
	virtual void StartPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void PostSeamlessTravel() override;

private: 

	int32 CountPlayers{0};
	int32 IndexPlayer{0};
};
