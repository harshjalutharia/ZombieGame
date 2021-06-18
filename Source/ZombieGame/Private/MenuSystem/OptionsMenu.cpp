// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/OptionsMenu.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Interfaces/ZINT_GameInstance.h"


bool UOptionsMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(ApplyGameplayButton!=nullptr)) return false;
	ApplyGameplayButton->OnClicked.AddDynamic(this, &UOptionsMenu::OnApplyGameplayButtonClicked);

	ShowGameplaySettingsWindow();

	return true;
}


void UOptionsMenu::Setup(IZINT_GameInstance* NewInterface)
{
	Super::Setup(NewInterface);

	SetMaxMinMouseSensitivity();
}


float UOptionsMenu::GetMinMouseSensitivityFromGameInstance()
{
	if(GameInstanceInterface != nullptr)
		return GameInstanceInterface->GetMinMouseSensitivity();
	
	return 1;
}


float UOptionsMenu::GetMaxMouseSensitivityFromGameInstance()
{
	if(GameInstanceInterface != nullptr)
		return GameInstanceInterface->GetMaxMouseSensitivity();
	
	return 100;
}


void UOptionsMenu::OnApplyGameplayButtonClicked()
{
	bool bToggleADS = false;
	if(ToggleADSCheckBox != nullptr)
		bToggleADS = ToggleADSCheckBox->IsChecked();
	
	GameplaySettings.MouseSensitivity = MouseSensitivity;
	GameplaySettings.bToggleADS = bToggleADS;
	
	if(GameInstanceInterface != nullptr)
		return GameInstanceInterface->SaveGameplaySettings(GameplaySettings);
}


void UOptionsMenu::ShowGameplaySettingsWindow()
{
	if(WindowSwitcher != nullptr && GameplayWindow != nullptr)
		WindowSwitcher->SetActiveWidget(GameplayWindow);

	if(GameInstanceInterface != nullptr)
		GameplaySettings = GameInstanceInterface->GetGameplaySettings();
	
	MouseSensitivity = GameplaySettings.MouseSensitivity;
	ToggleADSCheckBox->SetIsChecked(GameplaySettings.bToggleADS);
}


void UOptionsMenu::ShowAudioSettingsWindow()
{
	if(WindowSwitcher != nullptr && AudioWindow != nullptr)
		WindowSwitcher->SetActiveWidget(AudioWindow);
}

void UOptionsMenu::ShowVideoSettingsWindow()
{
	if(WindowSwitcher != nullptr && VideoWindow != nullptr)
		WindowSwitcher->SetActiveWidget(VideoWindow);
}


void UOptionsMenu::ShowControlsSettingsWindow()
{
	if(WindowSwitcher != nullptr && ControlsWindow != nullptr)
		WindowSwitcher->SetActiveWidget(ControlsWindow);
}


