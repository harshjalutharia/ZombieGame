// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/LobbySettingsMenu.h"
#include "MenuSystem/LobbyMenu.h"
#include "MenuSystem/HorizontalList.h"
#include "MenuSystem/KickPlayerButton.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"


bool ULobbySettingsMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(ApplySettingsButton!=nullptr)) return false;
	ApplySettingsButton->OnClicked.AddDynamic(this, &ULobbySettingsMenu::OnApplySettingsButtonClicked);

	if(WindowSwitcher != nullptr && SettingsWindow != nullptr)
		WindowSwitcher->SetActiveWidget(SettingsWindow);

	return true;
}


void ULobbySettingsMenu::Setup(IZINT_GameInstance* NewInterface)
{
	Super::Setup(NewInterface);
	
	if(GameInstanceInterface != nullptr)
	{
		GameInstanceInterface->GetAllGameModesAndMaps(HLS_GameMode->List,HLS_Map->List);
		GameInstanceInterface->GetAllTimeLimitsAndScoreLimits(HLS_TimeLimit->List,HLS_ScoreLimit->List);
		
		HLS_Map->Setup();
		HLS_GameMode->Setup();
		HLS_TimeLimit->Setup();
		HLS_ScoreLimit->Setup();
	}
}


void ULobbySettingsMenu::OnApplySettingsButtonClicked()
{
	if(GameInstanceInterface != nullptr)
	{
		const uint8 MapIndex = HLS_Map->GetCurrentIndex();
		const uint8 GameModeIndex = HLS_GameMode->GetCurrentIndex();
		const uint8 ScoreLimitIndex = HLS_ScoreLimit->GetCurrentIndex();
		const uint8 TimeLimitIndex = HLS_TimeLimit->GetCurrentIndex();

		GameInstanceInterface->HostUpdateLobbyServerInfo(GameModeIndex, MapIndex, ScoreLimitIndex, TimeLimitIndex);
	}
}


void ULobbySettingsMenu::ShowSettingsWindow()
{
	if(WindowSwitcher != nullptr && SettingsWindow != nullptr)
		WindowSwitcher->SetActiveWidget(SettingsWindow);

	if(GameInstanceInterface != nullptr)
	{
		uint8 MapIndex, GameModeIndex, TimeLimitIndex, ScoreLimitIndex;
		GameInstanceInterface->GetCurrentLobbyInfoIndexes(GameModeIndex,MapIndex, ScoreLimitIndex, TimeLimitIndex);
		HLS_Map->SetCurrentIndex(MapIndex);
		HLS_GameMode->SetCurrentIndex(GameModeIndex);
		HLS_ScoreLimit->SetCurrentIndex(ScoreLimitIndex);
		HLS_TimeLimit->SetCurrentIndex(TimeLimitIndex);
	}
}


void ULobbySettingsMenu::ShowKickPlayersWindow()
{
	if(WindowSwitcher != nullptr && KickPlayersWindow != nullptr)
		WindowSwitcher->SetActiveWidget(KickPlayersWindow);

	RefreshKickPlayersList();
}


void ULobbySettingsMenu::RefreshKickPlayersList()
{
	if(KickPlayersList != nullptr)
	{
		KickPlayersList->ClearChildren();
		auto AllPlayersInfo = LobbyMenuRef->GetAllPlayersInfo();
		int32 HostPlayerID = -1;
		if(LobbyMenuRef != nullptr)
			HostPlayerID = LobbyMenuRef->GetSelfPlayerID();
		
		for(auto& PlayerInfo : AllPlayersInfo)
		{
			if(PlayerInfo.PlayerID != HostPlayerID)
			{
				UKickPlayerButton* Player = CreateWidget<UKickPlayerButton>(this, KickPlayerButtonWidgetClass);
				if(Player != nullptr)
				{
					Player->SetPlayerInfo(PlayerInfo);
					KickPlayersList->AddChild(Player);
				}
			}
		}
	}
}

