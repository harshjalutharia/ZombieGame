// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/LobbyServerInfo.h"
#include "Structs/LobbyPlayerInfo.h"
#include "MenuSystem/MenuWidget.h"
#include "LobbyMenu.generated.h"

class ALobbyPlayerController;

class UImage;
class UButton;
class UTextBlock;
class UWidgetSwitcher;
class UScrollBox;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API ULobbyMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

public:

	virtual void Setup(IZINT_GameInstance* NewInterface) override;

	void SetPlayerControllerReference(ALobbyPlayerController* InLobbyPlayerController) { LobbyPlayerController = InLobbyPlayerController; }
	
private:

	UPROPERTY()
	ALobbyPlayerController* LobbyPlayerController;

	bool bIsHost;

	bool bIsReady;

	/*
	 *Lobby Settings Display
	 */
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	UImage* MapImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MapName;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameModeName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeLimit;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreLimit;

	/*
	 *Lobby Interactive Buttons
	 */

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* ButtonsListSwitcher;

			/*
			 *Start Menu
			 */

	UPROPERTY(meta = (BindWidget))
	UWidget* StartMenu;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ReadyStatusText;
	
	UPROPERTY(meta = (BindWidget))
	UButton* StartReadyButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StartReadyButtonText;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ChangeSettingsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SelectLoadoutButton;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitLobbyButton;

	UFUNCTION()
	void OnStartReadyButtonClicked();

	UFUNCTION()
	void OnChangeSettingsButtonClicked();

	UFUNCTION()
	void OnSelectLoadoutButtonClicked();

	UFUNCTION()
	void OnOptionsButtonClicked();

	UFUNCTION()
	void OnExitLobbyButtonClicked();

	UFUNCTION()
	void ShowStartMenu();

			/*
			 *Server Settings Menu
			 */

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* ServerSettingsMenu;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ChangeSettingsServerSettingsButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* KickPlayersButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BackServerSettingsButton;

	UFUNCTION()
	void OnChangeSettingsServerSettingsButtonClicked();

	UFUNCTION()
	void OnKickPlayersButtonClicked();

	UFUNCTION()
	void OnBackButtonClicked();

			/*
			 *Select Loadout Menu
			 */

	UPROPERTY(meta = (BindWidget))
	UWidget* SelectLoadoutMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* PrimaryWeaponButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SecondaryWeaponButton;

	UPROPERTY(meta = (BindWidget))
	UButton* BackChangeLoadoutButton;

	UFUNCTION()
	void OnPrimaryWeaponButtonClicked();

	UFUNCTION()
	void OnSecondaryWeaponButtonClicked();

			/*
			 *Options Menu
			 */

	UPROPERTY(meta = (BindWidget))
	UWidget* OptionsMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* GameplayButton;

	UPROPERTY(meta = (BindWidget))
	UButton* AudioButton;

	UPROPERTY(meta = (BindWidget))
	UButton* VideoButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ControlsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButtonOptionsMenu;

	UFUNCTION()
	void OnGameplayButtonClicked();

	UFUNCTION()
	void OnAudioButtonClicked();

	UFUNCTION()
	void OnVideoButtonClicked();

	UFUNCTION()
	void OnControlsButtonClicked();
	
	/*
	 *Connected Players list
	 */

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* PlayerList;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* PlayerListScrollBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UConnectedPlayer> ConnectedPlayerWidgetClass;

	/*
	 *Details Window
	 */

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* DetailsWindowSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* EmptyWindow;

	UPROPERTY(meta = (BindWidget))
	class ULobbySettingsMenu* ServerSettingsWindow;

	UPROPERTY(meta = (BindWidget))
	class USelectLoadoutMenu* SelectLoadoutWindow;

	UPROPERTY(meta = (BindWidget))
	class UOptionsMenu* OptionsWindow;

	/*
	 *Helper Functions
	 */

	void SetAsReady();

	void SetAsNotReady();

	void TryStartSession();

	void ClearPlayerList();

	void AddToPlayerList(FLobbyPlayerInfo NewPlayerInfo);
	
public:

	void UpdateLobbyInfo(FLobbyServerInfo InLobbyServerInfo);
};
