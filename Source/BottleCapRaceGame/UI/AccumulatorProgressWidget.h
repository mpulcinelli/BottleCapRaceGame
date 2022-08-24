// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "AccumulatorProgressWidget.generated.h"

/**
 *
 */
UCLASS()
class BOTTLECAPRACEGAME_API UAccumulatorProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual bool Initialize() override;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar *PBarAccumulator = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UHorizontalBox *HorizSlot = nullptr;

};
