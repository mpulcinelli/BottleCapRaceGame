// Fill out your copyright notice in the Description page of Project Settings.

#include "BottleCapPlayerState.h"
#include "Net/UnrealNetwork.h"

ABottleCapPlayerState::ABottleCapPlayerState()
{
    BottleCapPlayerName = "0";
}



void ABottleCapPlayerState::SetMyName(FString name)
{
    if (HasAuthority())
    {
        BottleCapPlayerName = name;
    }
}

FString ABottleCapPlayerState::GetMyName()
{
    return BottleCapPlayerName;
}

void ABottleCapPlayerState::OnRep_SetMyName()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_SetMyName - Player name: %s"), *BottleCapPlayerName);

}

void ABottleCapPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	UE_LOG(LogTemp, Warning, TEXT("ABottleCapPlayerState::GetLifetimeReplicatedProps"));

	DOREPLIFETIME(ABottleCapPlayerState, BottleCapPlayerName);
}
