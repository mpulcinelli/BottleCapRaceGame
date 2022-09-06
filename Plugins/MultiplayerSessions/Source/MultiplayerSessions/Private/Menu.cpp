// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;
    NumPublicConnections = NumberOfPublicConnections;
    MatchType = TypeOfMatch;
    PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

    UWorld *World = GetWorld();
    if (World)
    {
        auto PlayerController = World->GetFirstPlayerController();
        if (PlayerController)
        {
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }

    UGameInstance *GameInstance = GetGameInstance();

    if (GameInstance)
    {
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }

    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
        MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSessions);
        MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
        MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
    }
}

void UMenu::HostButtonClicked()
{
    HostButton->SetIsEnabled(false);
    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString("UMenu::HostButtonClicked()"));
        }
    }
}

void UMenu::JoinButtonClicked()
{
    JoinButton->SetIsEnabled(false);

    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->FindSessions(100000);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString("UMenu::JoinButtonClicked()"));
        }
    }
}

bool UMenu::Initialize()
{

    if (!Super::Initialize())
        return false;

    if (HostButton)
    {
        HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
    }

    if (JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
    }

    return true;
}

void UMenu::MenuTearDown()
{
    RemoveFromParent();
    UWorld *World = GetWorld();
    if (World)
    {
        APlayerController *PlayerController = World->GetFirstPlayerController();
        FInputModeGameOnly InputModeData;
        PlayerController->SetInputMode(InputModeData);
        PlayerController->SetShowMouseCursor(false);
    }
}

void UMenu::OnLevelRemovedFromWorld(ULevel *InLevel, UWorld *InWorld)
{
    MenuTearDown();
    Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSession(bool bWasSuccess)
{
    if (bWasSuccess)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString("SESSION CREATED!"));
        }

        UWorld *World = GetWorld();
        if (World)
        {
            World->ServerTravel(PathToLobby);
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString("FAILD TO CREATE SESSION!"));
        }
        HostButton->SetIsEnabled(true);
    }
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult> &SessionResult, bool bWasSuccessful)
{

    if (MultiplayerSessionsSubsystem == nullptr)
        return;

    for (auto Result : SessionResult)
    {
        FString Id = Result.GetSessionIdStr();
        FString User = Result.Session.OwningUserName;

        FString SettingValue;
        Result.Session.SessionSettings.Get(FName("MatchType"), SettingValue);

        if (SettingValue == MatchType)
        {
            MultiplayerSessionsSubsystem->JoinSession(Result);
            return;
        }
    }
    if (!bWasSuccessful || SessionResult.Num() == 0)
    {
        JoinButton->SetIsEnabled(true);
    }
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{

    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            FString Address;
            if (SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Connect String: %s"), *Address));
                }

                APlayerController *PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
                if (PlayerController)
                {
                    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
                }
            }
        }
    }

    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        JoinButton->SetIsEnabled(true);
    }
}

void UMenu::OnDestroySession(bool bWasSuccess)
{
}

void UMenu::OnStartSession(bool bWasSuccess)
{
}
