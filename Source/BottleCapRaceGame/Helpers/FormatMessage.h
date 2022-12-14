// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DEFINE_LOG_CATEGORY_STATIC(LogBottleCapRace, Log, All);

#define PRINT_LOG() UE_LOG(LogBottleCapRace, Warning, TEXT("[ %s ]"), *FString(__PRETTY_FUNCTION__));
#define PRINT_LOG_1(TOKEN1) UE_LOG(LogBottleCapRace, Warning, TEXT("[ %s ]  [ %s ]"), *FString(__PRETTY_FUNCTION__), *FString(TOKEN1));
#define PRINT_LOG_2(TOKEN1, TOKEN2) UE_LOG(LogBottleCapRace, Warning, TEXT("[ %s ]  [ %s ] [ %s ]"), *FString(__PRETTY_FUNCTION__), *FString(TOKEN1), *FString(TOKEN2));
