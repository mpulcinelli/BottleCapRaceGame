// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BottleCapPlayerState.generated.h"

/**
 *
 */
UCLASS()
class BOTTLECAPRACEGAME_API ABottleCapPlayerState : public APlayerState
{
	GENERATED_BODY()


	ABottleCapPlayerState();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_SetMyName)
	FString BottleCapPlayerName;

	UFUNCTION()
	void OnRep_SetMyName();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

public:
	void SetMyName(FString name);
	FString GetMyName();
};
