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


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

};
