// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/LobbyMenu.h"
#include "MenuSystem/LobbySettingsMenu.h"
#include "MenuSystem/SelectLoadoutMenu.h"
#include "MenuSystem/OptionsMenu.h"
#include "MenuSystem/ConnectedPlayer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "LobbyPlayerController.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "MenuSystem/ChatWindow.h"
#include "LobbyGameMode.h"


bool ULobbyMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	/*
	 *Start Menu
	 */

	if(!ensure(StartReadyButton!=nullptr)) return false;
	StartReadyButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnStartReadyButtonClicked);
	
	if(!ensure(ChangeSettingsButton!=nullptr)) return false;
	ChangeSettingsButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnChangeSettingsButtonClicked);

	if(!ensure(SelectLoadoutButton!=nullptr)) return false;
	SelectLoadoutButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnSelectLoadoutButtonClicked);

	if(!ensure(OptionsButton!=nullptr)) return false;
	OptionsButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnOptionsButtonClicked);

	if(!ensure(ExitLobbyButton!=nullptr)) return false;
	ExitLobbyButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnExitLobbyButtonClicked);

	/*
	 *Server Settings Menu
	 */

	if(!ensure(ChangeSettingsServerSettingsButton!=nullptr)) return false;
	ChangeSettingsServerSettingsButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnChangeSettingsServerSettingsButtonClicked);

	if(!ensure(KickPlayersButton!=nullptr)) return false;
	KickPlayersButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnKickPlayersButtonClicked);

	if(!ensure(BackServerSettingsButton!=nullptr)) return false;
	BackServerSettingsButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnBackButtonClicked);

	/*
	 *Select Loadout Menu
	 */

	if(!ensure(PrimaryWeaponButton!=nullptr)) return false;
	PrimaryWeaponButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnPrimaryWeaponButtonClicked);

	if(!ensure(SecondaryWeaponButton!=nullptr)) return false;
	SecondaryWeaponButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnSecondaryWeaponButtonClicked);

	if(!ensure(BackChangeLoadoutButton!=nullptr)) return false;
	BackChangeLoadoutButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnBackButtonClicked);

	/*
	*Options Menu
	*/

	if(!ensure(GameplayButton!=nullptr)) return false;
	GameplayButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnGameplayButtonClicked);

	if(!ensure(AudioButton!=nullptr)) return false;
	AudioButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnAudioButtonClicked);

	if(!ensure(VideoButton!=nullptr)) return false;
	VideoButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnVideoButtonClicked);

	if(!ensure(ControlsButton!=nullptr)) return false;
	ControlsButton->OnClicked.AddDynamic(this, &ULobbyMenu::OnControlsButtonClicked);

	if(!ensure(BackButtonOptionsMenu!=nullptr)) return false;
	BackButtonOptionsMenu->OnClicked.AddDynamic(this, &ULobbyMenu::OnBackButtonClicked);

	ShowStartMenu();
	bIsReady = false;

	return true;
}


void ULobbyMenu::Setup(IZINT_GameInstance* NewInterface)
{
	Super::Setup(NewInterface);

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		if (World->IsServer())
		{
			bIsHost = true;
			bIsReady = true;

			if(ChangeSettingsButton != nullptr)
				ChangeSettingsButton->SetVisibility(ESlateVisibility::Visible);
			if(StartReadyButtonText != nullptr)
				StartReadyButtonText->SetText(FText::FromString("Start Game"));
			if(ReadyStatusText != nullptr)
				ReadyStatusText->SetText(FText::FromString("Players Ready: 1/X"));
		}
		else
		{
			bIsHost = false;
			bIsReady = false;

			if(ChangeSettingsButton != nullptr)
				ChangeSettingsButton->SetVisibility(ESlateVisibility::Hidden);
			if(StartReadyButtonText != nullptr)
				StartReadyButtonText->SetText(FText::FromString("Ready"));
			if(ReadyStatusText != nullptr)
				ReadyStatusText->SetText(FText::FromString("Status: Not Ready"));
		}
	}

	if(ServerSettingsWindow != nullptr)
	{
		ServerSettingsWindow->Setup(NewInterface);
		ServerSettingsWindow->SetLobbyMenuReference(this);
	}

	if(SelectLoadoutWindow != nullptr)
		SelectLoadoutWindow->Setup(NewInterface);

	if(OptionsWindow != nullptr)
		OptionsWindow->Setup(NewInterface);

	if(ChatWindow != nullptr)
		ChatWindow->Setup(NewInterface);
}


void ULobbyMenu::SetPlayerControllerReference(ALobbyPlayerController* InLobbyPlayerController)
{
	LobbyPlayerController = InLobbyPlayerController;
	
	if(ChatWindow != nullptr)
		ChatWindow->SetLobbyPlayerControllerReference(LobbyPlayerController);
}


void ULobbyMenu::OnStartReadyButtonClicked()
{
	if(bIsHost)
	{
		TryStartSession();
	}
	else
	{
		if(bIsReady)
			SetAsNotReady(true);
		else
			SetAsReady(true);
	}
}


void ULobbyMenu::OnChangeSettingsButtonClicked()
{
	if(ButtonsListSwitcher != nullptr && ServerSettingsMenu != nullptr)
		ButtonsListSwitcher->SetActiveWidget(ServerSettingsMenu);

	if(DetailsWindowSwitcher != nullptr && ServerSettingsWindow != nullptr)
		DetailsWindowSwitcher->SetActiveWidget(ServerSettingsWindow);

	if(ServerSettingsWindow != nullptr)
		ServerSettingsWindow->ShowSettingsWindow();

	SetAsNotReady(true);
}


void ULobbyMenu::OnSelectLoadoutButtonClicked()
{
	if(ButtonsListSwitcher != nullptr && SelectLoadoutMenu != nullptr)
		ButtonsListSwitcher->SetActiveWidget(SelectLoadoutMenu);

	if(DetailsWindowSwitcher != nullptr && SelectLoadoutWindow != nullptr)
		DetailsWindowSwitcher->SetActiveWidget(SelectLoadoutWindow);

	if(SelectLoadoutWindow != nullptr)
		SelectLoadoutWindow->ShowPrimaryLoadoutWindow();

	SetAsNotReady(true);
}


void ULobbyMenu::OnOptionsButtonClicked()
{
	if(ButtonsListSwitcher != nullptr && OptionsMenu != nullptr)
		ButtonsListSwitcher->SetActiveWidget(OptionsMenu);

	if(DetailsWindowSwitcher != nullptr && OptionsWindow != nullptr)
		DetailsWindowSwitcher->SetActiveWidget(OptionsWindow);

	if(OptionsWindow != nullptr)
		OptionsWindow->ShowGameplaySettingsWindow();

	SetAsNotReady(true);
}


void ULobbyMenu::OnExitLobbyButtonClicked()
{
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		if (World->IsServer())
		{
			AGameModeBase* GameMode = World->GetAuthGameMode();
			if (GameMode != nullptr)
			{
				GameMode->ReturnToMainMenuHost();
			}
		}
		else
		{
			APlayerController* PC = GetOwningPlayer();
			if (PC != nullptr)
			{
				PC->ClientReturnToMainMenuWithTextReason(FText::FromString("Player quit game"));
			}
		}
	}
}


void ULobbyMenu::ShowStartMenu()
{
	if(ButtonsListSwitcher != nullptr && StartMenu != nullptr)
		ButtonsListSwitcher->SetActiveWidget(StartMenu);

	if(DetailsWindowSwitcher != nullptr && EmptyWindow != nullptr)
		DetailsWindowSwitcher->SetActiveWidget(EmptyWindow);
}


void ULobbyMenu::OnChangeSettingsServerSettingsButtonClicked()
{
	if(ServerSettingsWindow != nullptr)
		ServerSettingsWindow->ShowSettingsWindow();
}


void ULobbyMenu::OnKickPlayersButtonClicked()
{
	if(ServerSettingsWindow != nullptr)
		ServerSettingsWindow->ShowKickPlayersWindow();
}


void ULobbyMenu::OnBackButtonClicked()
{
	ShowStartMenu();
}


void ULobbyMenu::OnPrimaryWeaponButtonClicked()
{
	if(SelectLoadoutWindow != nullptr)
		SelectLoadoutWindow->ShowPrimaryLoadoutWindow();
}


void ULobbyMenu::OnSecondaryWeaponButtonClicked()
{
	if(SelectLoadoutWindow != nullptr)
		SelectLoadoutWindow->ShowSecondaryLoadoutWindow();
}


void ULobbyMenu::OnGameplayButtonClicked()
{
	if(OptionsWindow != nullptr)
		OptionsWindow->ShowGameplaySettingsWindow();
}


void ULobbyMenu::OnAudioButtonClicked()
{
	if(OptionsWindow != nullptr)
		OptionsWindow->ShowAudioSettingsWindow();
}


void ULobbyMenu::OnVideoButtonClicked()
{
	if(OptionsWindow != nullptr)
		OptionsWindow->ShowVideoSettingsWindow();
}


void ULobbyMenu::OnControlsButtonClicked()
{
	if(OptionsWindow != nullptr)
		OptionsWindow->ShowControlsSettingsWindow();
}


void ULobbyMenu::SetAsReady(bool bInformServer)
{
	if(bIsHost) return;
	
	bIsReady = true;
	if(StartReadyButtonText != nullptr)
		StartReadyButtonText->SetText(FText::FromString("Unready"));
	if(ReadyStatusText != nullptr)
		ReadyStatusText->SetText(FText::FromString("Status: Ready"));

	if(bInformServer && LobbyPlayerController != nullptr)
		LobbyPlayerController->UpdateStatus(true);
}


void ULobbyMenu::SetAsNotReady(bool bInformServer)
{
	if(bIsHost) return;
	
	bIsReady = false;
	if(StartReadyButtonText != nullptr)
		StartReadyButtonText->SetText(FText::FromString("Ready"));
	if(ReadyStatusText != nullptr)
		ReadyStatusText->SetText(FText::FromString("Status: Not Ready"));

	if(bInformServer && LobbyPlayerController != nullptr)
		LobbyPlayerController->UpdateStatus(false);
}


void ULobbyMenu::TryStartSession()
{
	if(!bIsHost) return;

	bool bCanStart = true;

	for(auto& PlayerInfo : AllPlayersInfo)
	{
		if(!PlayerInfo.bIsReady)
		{
			bCanStart = false;
			break;
		}
	}

	if(bCanStart && GameInstanceInterface != nullptr)
	{
		ALobbyGameMode* GameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
		if(GameMode != nullptr)
			GameMode->StartGame();
	}
}


void ULobbyMenu::ClearPlayerList()
{
	if(PlayerListScrollBox != nullptr)
		PlayerListScrollBox->ClearChildren();
}


void ULobbyMenu::AddToPlayerList(FLobbyPlayerInfo NewPlayerInfo)
{
	if(ConnectedPlayerWidgetClass == nullptr || PlayerListScrollBox == nullptr) return;
	
	UConnectedPlayer* NewPlayer = CreateWidget<UConnectedPlayer>(GetOwningPlayer(),ConnectedPlayerWidgetClass);
	if(NewPlayer)
	{
		PlayerListScrollBox->AddChild(NewPlayer);
		NewPlayer->SetPlayerStatus(NewPlayerInfo.bIsReady);
		NewPlayer->SetPlayerNameAndAvatar(NewPlayerInfo.PlayerName, NewPlayerInfo.PlayerAvatarImage);
	}
}


void ULobbyMenu::UpdateReadyStatusTextOnHost()
{
	if(ReadyStatusText != nullptr)
	{
		int32 ReadyPlayerCount = 0;
		int32 TotalPlayerCount = 0;

		for(auto& PlayerInfo : AllPlayersInfo)
		{
			TotalPlayerCount++;
			if(PlayerInfo.bIsReady)
				ReadyPlayerCount++;
		}
		
		ReadyStatusText->SetText(FText::FromString(FString::Printf(TEXT("Players Ready: %d/%d"),ReadyPlayerCount,TotalPlayerCount)));
	}
}


void ULobbyMenu::UpdateLobbyInfo(FLobbyServerInfo InLobbyServerInfo)
{
	if(ServerName != nullptr)
		ServerName->SetText(FText::FromString(InLobbyServerInfo.ServerName));
	
	if(MapName != nullptr)
		MapName->SetText(FText::FromString(InLobbyServerInfo.MapName));

	if(GameModeName != nullptr)
		GameModeName->SetText(FText::FromString(InLobbyServerInfo.GameModeName));

	if(TimeLimit != nullptr)
		TimeLimit->SetText(FText::FromString(InLobbyServerInfo.TimeLimit));

	if(ScoreLimit != nullptr)
		ScoreLimit->SetText(FText::FromString(InLobbyServerInfo.ScoreLimit));
}


void ULobbyMenu::UpdateAllPlayersInfo(const TArray<FLobbyPlayerInfo>& InLobbyPlayerInfo)
{
	if(PlayerListScrollBox != nullptr && IsValid(ConnectedPlayerWidgetClass))
	{
		AllPlayersInfo = InLobbyPlayerInfo;
		
		PlayerListScrollBox->ClearChildren();
		
		for(auto &PlayerInfo : AllPlayersInfo)
		{
			UConnectedPlayer* Player = CreateWidget<UConnectedPlayer>(this,ConnectedPlayerWidgetClass);
			if(Player != nullptr)
			{
				Player->SetPlayerNameAndAvatar(PlayerInfo.PlayerName, PlayerInfo.PlayerAvatarImage);
				Player->SetPlayerStatus(PlayerInfo.bIsReady);
				PlayerListScrollBox->AddChild(Player);
			}

			if(!bIsHost && PlayerInfo.PlayerID == GetSelfPlayerID())
			{
				if(PlayerInfo.bIsReady)
					SetAsReady(false);
				else
					SetAsNotReady(false);
			}
		}

		if(ServerSettingsWindow != nullptr)
			ServerSettingsWindow->RefreshKickPlayersList();

		if(bIsHost)
			UpdateReadyStatusTextOnHost();
	}
}


int32 ULobbyMenu::GetSelfPlayerID() const
{
	if(LobbyPlayerController != nullptr)
		return LobbyPlayerController->GetSelfPlayerID();

	return -1;
}


void ULobbyMenu::AddMessageToChatWindow(const FString& PlayerName, const FString& Message, const EChatLogType ChatLogType)
{
	if(ChatWindow != nullptr)
	{
		ChatWindow->AddMessageToChatWindow(PlayerName,Message, ChatLogType);
	}
}
