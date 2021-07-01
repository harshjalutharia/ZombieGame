// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/PauseMenu.h"

#include "ZPlayerController.h"
#include "MenuSystem/OptionsMenu.h"
#include "GameFramework/GameModeBase.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"


bool UPauseMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if (!Success) return false;

	/*
	 *Start Menu
	 */

	if (!ensure(ResumeButton != nullptr)) return false;
	ResumeButton->OnClicked.AddDynamic(this, &UPauseMenu::OnResumeButtonClicked);

	if (!ensure(OptionsButton != nullptr)) return false;
	OptionsButton->OnClicked.AddDynamic(this, &UPauseMenu::OnOptionsButtonClicked);

	if (!ensure(ExitButton != nullptr)) return false;
	ExitButton->OnClicked.AddDynamic(this, &UPauseMenu::OnExitButtonClicked);

	/*
	 *Options Menu
	 */

	if(!ensure(GameplayButton!=nullptr)) return false;
	GameplayButton->OnClicked.AddDynamic(this, &UPauseMenu::OnGameplayButtonClicked);

	if(!ensure(AudioButton!=nullptr)) return false;
	AudioButton->OnClicked.AddDynamic(this, &UPauseMenu::OnAudioButtonClicked);

	if(!ensure(VideoButton!=nullptr)) return false;
	VideoButton->OnClicked.AddDynamic(this, &UPauseMenu::OnVideoButtonClicked);

	if(!ensure(ControlsButton!=nullptr)) return false;
	ControlsButton->OnClicked.AddDynamic(this, &UPauseMenu::OnControlsButtonClicked);

	if(!ensure(BackButtonOptionsMenu!=nullptr)) return false;
	BackButtonOptionsMenu->OnClicked.AddDynamic(this, &UPauseMenu::OnBackButtonClicked);

	return true;
}


void UPauseMenu::Setup(IZINT_GameInstance* NewInterface)
{
	Super::Setup(NewInterface);

	OptionsWindow->Setup(NewInterface);
}


void UPauseMenu::ShowPauseMenu()
{
	if(!IsInViewport())
	{
		AddToViewport();
		bIsFocusable = true;
		
		auto PC = GetOwningPlayer();
		if(!ensure(PC!=nullptr)) return;

		PC->bShowMouseCursor = true;
		
		if(GEngine && GEngine->GameViewport)
		{
			FVector2D ViewportSize = FVector2D(1.f);
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			PC->SetMouseLocation(ViewportSize.X/2, ViewportSize.Y/2);
		}

		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputModeData);

		if(WindowSwitcher != nullptr && StartMenu != nullptr)
			WindowSwitcher->SetActiveWidget(StartMenu);
	}
}


void UPauseMenu::HidePauseMenu()
{
	if(IsInViewport())
	{
		RemoveFromViewport();
		bIsFocusable = false;

		auto PC = GetOwningPlayer();
		if(!ensure(PC!=nullptr)) return;

		PC->bShowMouseCursor = false;

		const FInputModeGameOnly InputModeData;
		PC->SetInputMode(InputModeData);
	}
}


void UPauseMenu::OnResumeButtonClicked()
{
	AZPlayerController* PlayerController = Cast<AZPlayerController>(GetOwningPlayer());
	if(PlayerController != nullptr)
		PlayerController->TogglePauseMenu();
}


void UPauseMenu::OnOptionsButtonClicked()
{
	if(WindowSwitcher != nullptr && OptionsMenu != nullptr)
		WindowSwitcher->SetActiveWidget(OptionsMenu);

	if(OptionsWindow != nullptr)
		OptionsWindow->ShowGameplaySettingsWindow();
}


void UPauseMenu::OnExitButtonClicked()
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


void UPauseMenu::OnGameplayButtonClicked()
{
	if(OptionsWindow != nullptr)
		OptionsWindow->ShowGameplaySettingsWindow();
}


void UPauseMenu::OnAudioButtonClicked()
{
	if(OptionsWindow != nullptr)
		OptionsWindow->ShowAudioSettingsWindow();
}


void UPauseMenu::OnVideoButtonClicked()
{
	if(OptionsWindow != nullptr)
		OptionsWindow->ShowVideoSettingsWindow();
}


void UPauseMenu::OnControlsButtonClicked()
{
	if(OptionsWindow != nullptr)
		OptionsWindow->ShowControlsSettingsWindow();
}


void UPauseMenu::OnBackButtonClicked()
{
	if(WindowSwitcher != nullptr && StartMenu != nullptr)
		WindowSwitcher->SetActiveWidget(StartMenu);
}


