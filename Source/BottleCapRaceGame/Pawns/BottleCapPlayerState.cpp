// Fill out your copyright notice in the Description page of Project Settings.

#include "BottleCapPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "BottleCapRaceGame/Helpers/FormatMessage.h"


void ABottleCapPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    PRINT_LOG();
}
