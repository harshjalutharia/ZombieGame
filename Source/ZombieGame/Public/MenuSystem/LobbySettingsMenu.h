// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "LobbySettingsMenu.generated.h"

class UWidgetSwitcher;
class UScrollBox;
class UHorizontalList;
class UButton;
class ULobbyMenu;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API ULobbySettingsMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

public:

	virtual void Setup(IZINT_GameInstance* NewInterface) override;
	
private:

	UPROPERTY()
	ULobbyMenu* LobbyMenuRef;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WindowSwitcher;

	/*
	*Server Settings Window
	*/

	UPROPERTY(meta = (BindWidget))
	UWidget* SettingsWindow;

	UPROPERTY(meta = (BindWidget))
	UHorizontalList* HLS_GameMode;

	UPROPERTY(meta = (BindWidget))
	UHorizontalList* HLS_Map;

	UPROPERTY(meta = (BindWidget))
	UHorizontalList* HLS_TimeLimit;

	UPROPERTY(meta = (BindWidget))
	UHorizontalList* HLS_ScoreLimit;

	UPROPERTY(meta = (BindWidget))
	UButton* ApplySettingsButton;

	UFUNCTION()
	void OnApplySettingsButtonClicked();

	/*
	 *Kick Players Window
	 */

	UPROPERTY(meta = (BindWidget))
	UWidget* KickPlayersWindow;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* KickPlayersList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UKickPlayerButton> KickPlayerButtonWidgetClass;

public:

	void ShowSettingsWindow();

	void ShowKickPlayersWindow();

	void RefreshKickPlayersList();
	
	void SetLobbyMenuReference(ULobbyMenu* InLobbyMenuRef) { LobbyMenuRef = InLobbyMenuRef; }
	
};
