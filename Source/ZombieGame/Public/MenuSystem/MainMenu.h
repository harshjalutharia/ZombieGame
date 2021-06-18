// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "MainMenu.generated.h"

class UButton;
class UWidgetSwitcher;
class UOverlay;
class UEditableText;
class UTextBlock;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

public:

	virtual void Setup(IZINT_GameInstance* NewInterface) override;

private:

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* DetailsWindowSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* EmptyWindow;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;
	
	UFUNCTION()
	void ExitGame();

	/*
	 *START MENU
	 */

	UPROPERTY(meta = (BindWidget))
	UWidget* StartMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* PlayButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CustomizeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CreditsButton;

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnCustomizeButtonClicked();

	UFUNCTION()
	void OnOptionsButtonClicked();

	UFUNCTION()
	void OnCreditsButtonClicked();

	void ShowStartMenu();

	/*
	 *PLAY MENU
	 */

	UPROPERTY(meta = (BindWidget))
	UWidget* PlayMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* HostGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* FindGamesButton;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButtonPlayMenu;

	UFUNCTION()
	void OnHostGameButtonClicked();

	UFUNCTION()
	void OnFindGamesButtonClicked();

	UFUNCTION()
	void OnBackButtonClicked();

	/*
	 *CUSTOMIZE MENU
	 */

	UPROPERTY(meta = (BindWidget))
	UWidget* CustomizeMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* FaceButton;

	UPROPERTY(meta = (BindWidget))
	UButton* BodyButton;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButtonCustomizeMenu;

	UFUNCTION()
	void OnFaceButtonClicked();

	UFUNCTION()
	void OnBodyButtonClicked();
	
	/*
	 *OPTIONS MENU
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
	 *CREDITS MENU
	 */

	UPROPERTY(meta = (BindWidget))
	UWidget* CreditsMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButtonCreditsMenu;

	/*
	 *Detail Windows
	 */

	UPROPERTY(meta = (BindWidget))
	class UHostMenu* HostMenuWindow;

	UPROPERTY(meta = (BindWidget))
	class UFindGamesMenu* FindGamesMenuWindow;

	UPROPERTY(meta = (BindWidget))
	class UOptionsMenu* OptionsMenuWindow;

	/*
	Popup UI
	*/

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ErrorPopupUI;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ErrorPopupText;

	UPROPERTY(meta = (BindWidget))
	UButton* ErrorPopupButton;

	UFUNCTION()
	void OnErrorPopupButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* LoadingPopupUI;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadingPopupText;	

public:

	void ShowErrorMessage(FString ErrorMessage);

	void ToggleLoadingMessage(bool bVisible, FString LoadingMessage = "");
};
