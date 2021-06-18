// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "Structs/GameplaySettings.h"
#include "OptionsMenu.generated.h"

class UWidgetSwitcher;
class UButton;
class UCheckBox;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UOptionsMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

public:

	virtual void Setup(IZINT_GameInstance* NewInterface) override;

private:

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WindowSwitcher;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* GameplayWindow;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* AudioWindow;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* VideoWindow;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ControlsWindow;

	/*
	 *Gameplay Window
	 */

protected:

	FGameplaySettings GameplaySettings;

	UPROPERTY(BlueprintReadWrite)
	float MouseSensitivity;

	UFUNCTION(BlueprintCallable)
	float GetMinMouseSensitivityFromGameInstance();

	UFUNCTION(BlueprintCallable)
	float GetMaxMouseSensitivityFromGameInstance();

	UFUNCTION(BlueprintImplementableEvent)
	void SetMaxMinMouseSensitivity();

private:

	UPROPERTY(meta = (BindWidget))
	UCheckBox* ToggleADSCheckBox;

	UPROPERTY(meta = (BindWidget))
	UButton* ApplyGameplayButton;

	UFUNCTION()
	void OnApplyGameplayButtonClicked();

public:

	void ShowGameplaySettingsWindow();

	void ShowAudioSettingsWindow();

	void ShowVideoSettingsWindow();

	void ShowControlsSettingsWindow();
};
