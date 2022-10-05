// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuPlayerPawn.h"
#include "BottleCapRaceGame/Helpers/FormatMessage.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "BottleCapRaceGame/BottleCapGameInstance.h"

/**
 * @brief Construct a new AMenuPlayerPawn::AMenuPlayerPawn object
 * 
 */
AMenuPlayerPawn::AMenuPlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG();
#endif
}


void AMenuPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG();
#endif
}

// Called to bind functionality to input
void AMenuPlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

#if UE_BUILD_DEVELOPMENT
	PRINT_LOG();
#endif
}
