// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections=5, FString TypeOfMatch=FString("FreeForAll"), FString LobbyPath=FString("/Game/ThirdPersonCPP/Maps/Lobby"));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel *InLevel, UWorld *InWorld) override;

	UFUNCTION()
	void OnCreateSession(bool bWasSuccess);

	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccessful);

	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccess);

	UFUNCTION()
	void OnStartSession(bool bWasSuccess);

private:
	UPROPERTY(meta = (BindWidget))
	class UButton *HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton *JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	class UMultiplayerSessionsSubsystem *MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{4};
	FString MatchType{TEXT("FreeForAll")};
	FString PathToLobby{TEXT("")};

};
