// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "PauseMenu.generated.h"

class UWidgetSwitcher;
class UButton;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UPauseMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

public:

	virtual void Setup(IZINT_GameInstance* NewInterface) override;

private:

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WindowSwitcher;

	/*
	 *Start Menu
	 */

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* StartMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* OptionsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UFUNCTION()
    void OnResumeButtonClicked();

	UFUNCTION()
	void OnOptionsButtonClicked();

    UFUNCTION()
    void OnExitButtonClicked();

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

	UFUNCTION()
	void OnBackButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	class UOptionsMenu* OptionsWindow;
	
};
