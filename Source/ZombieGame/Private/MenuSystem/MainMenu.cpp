// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "MenuSystem/HostMenu.h"
#include "MenuSystem/FindGamesMenu.h"


bool UMainMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(QuitButton!=nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::ExitGame);

	if(!ensure(ErrorPopupButton!=nullptr)) return false;
	ErrorPopupButton->OnClicked.AddDynamic(this, &UMainMenu::OnErrorPopupButtonClicked);

	if(ErrorPopupUI != nullptr)
		ErrorPopupUI->SetVisibility(ESlateVisibility::Hidden);

	if(LoadingPopupUI != nullptr)
		LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);

	/*
	*START MENU
	*/

	if(!ensure(PlayButton!=nullptr)) return false;
	PlayButton->OnClicked.AddDynamic(this, &UMainMenu::OnPlayButtonClicked);

	if(!ensure(CustomizeButton!=nullptr)) return false;
	CustomizeButton->OnClicked.AddDynamic(this, &UMainMenu::OnCustomizeButtonClicked);

	if(!ensure(OptionsButton!=nullptr)) return false;
	OptionsButton->OnClicked.AddDynamic(this, &UMainMenu::OnOptionsButtonClicked);

	if(!ensure(CreditsButton!=nullptr)) return false;
	CreditsButton->OnClicked.AddDynamic(this, &UMainMenu::OnCreditsButtonClicked);

	/*
	*PLAY MENU
	*/

	if(!ensure(HostGameButton!=nullptr)) return false;
	HostGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostGameButtonClicked);

	if(!ensure(FindGamesButton!=nullptr)) return false;
	FindGamesButton->OnClicked.AddDynamic(this, &UMainMenu::OnFindGamesButtonClicked);

	if(!ensure(BackButtonPlayMenu!=nullptr)) return false;
	BackButtonPlayMenu->OnClicked.AddDynamic(this, &UMainMenu::OnBackButtonClicked);

	/*
	*CUSTOMIZE MENU
	*/

	if(!ensure(FaceButton!=nullptr)) return false;
	FaceButton->OnClicked.AddDynamic(this, &UMainMenu::OnFaceButtonClicked);

	if(!ensure(BodyButton!=nullptr)) return false;
	BodyButton->OnClicked.AddDynamic(this, &UMainMenu::OnBodyButtonClicked);

	if(!ensure(BackButtonCustomizeMenu!=nullptr)) return false;
	BackButtonCustomizeMenu->OnClicked.AddDynamic(this, &UMainMenu::OnBackButtonClicked);
	
	/*
	*OPTIONS MENU
	*/

	if(!ensure(GameplayButton!=nullptr)) return false;
	GameplayButton->OnClicked.AddDynamic(this, &UMainMenu::OnGameplayButtonClicked);

	if(!ensure(AudioButton!=nullptr)) return false;
	AudioButton->OnClicked.AddDynamic(this, &UMainMenu::OnAudioButtonClicked);

	if(!ensure(VideoButton!=nullptr)) return false;
	VideoButton->OnClicked.AddDynamic(this, &UMainMenu::OnVideoButtonClicked);

	if(!ensure(ControlsButton!=nullptr)) return false;
	ControlsButton->OnClicked.AddDynamic(this, &UMainMenu::OnControlsButtonClicked);

	if(!ensure(BackButtonOptionsMenu!=nullptr)) return false;
	BackButtonOptionsMenu->OnClicked.AddDynamic(this, &UMainMenu::OnBackButtonClicked);

	/*
	*CREDITS MENU
	*/

	if(!ensure(BackButtonCreditsMenu!=nullptr)) return false;
	BackButtonCreditsMenu->OnClicked.AddDynamic(this, &UMainMenu::OnBackButtonClicked);
	
	if(MenuSwitcher != nullptr && StartMenu != nullptr)
		MenuSwitcher->SetActiveWidget(StartMenu);

	if(DetailsWindowSwitcher != nullptr && EmptyMenu != nullptr)
		DetailsWindowSwitcher->SetActiveWidget(EmptyMenu);
	
	return true;
}


void UMainMenu::Setup(IZINT_GameInstance* NewInterface)
{
	Super::Setup(NewInterface);

	HostMenu->Setup(NewInterface);
	FindGamesMenu->Setup(NewInterface);
}


void UMainMenu::ExitGame()
{
	auto PC = GetOwningPlayer();
	if(PC!=nullptr)
		PC->ConsoleCommand("Quit");
}


void UMainMenu::OnPlayButtonClicked()
{
	if(MenuSwitcher != nullptr && PlayMenu != nullptr)
	{
		MenuSwitcher->SetActiveWidget(PlayMenu);
		ShowHostMenu();
	}
}


void UMainMenu::OnCustomizeButtonClicked()
{
	if(MenuSwitcher != nullptr && CustomizeMenu != nullptr)
		MenuSwitcher->SetActiveWidget(CustomizeMenu);
}


void UMainMenu::OnOptionsButtonClicked()
{
	if(MenuSwitcher != nullptr && OptionsMenu != nullptr)
		MenuSwitcher->SetActiveWidget(OptionsMenu);
}


void UMainMenu::OnCreditsButtonClicked()
{
	if(MenuSwitcher != nullptr && CreditsMenu != nullptr)
		MenuSwitcher->SetActiveWidget(CreditsMenu);
}


void UMainMenu::ShowStartMenu()
{
	if(MenuSwitcher != nullptr && StartMenu != nullptr)
		MenuSwitcher->SetActiveWidget(StartMenu);

	if(DetailsWindowSwitcher != nullptr && EmptyMenu != nullptr)
		DetailsWindowSwitcher->SetActiveWidget(EmptyMenu);
}


void UMainMenu::OnHostGameButtonClicked()
{
	ShowHostMenu();
}


void UMainMenu::OnFindGamesButtonClicked()
{
	ShowFindGamesMenu();
}


void UMainMenu::OnBackButtonClicked()
{
	ShowStartMenu();
}


void UMainMenu::OnFaceButtonClicked()
{
}


void UMainMenu::OnBodyButtonClicked()
{
}


void UMainMenu::OnGameplayButtonClicked()
{
}


void UMainMenu::OnAudioButtonClicked()
{
}


void UMainMenu::OnVideoButtonClicked()
{
}


void UMainMenu::OnControlsButtonClicked()
{
}


void UMainMenu::ShowHostMenu()
{
	if(DetailsWindowSwitcher != nullptr && HostMenu != nullptr)
	{
		DetailsWindowSwitcher->SetActiveWidget(HostMenu);
	}
}


void UMainMenu::ShowFindGamesMenu()
{
	if(DetailsWindowSwitcher != nullptr && FindGamesMenu != nullptr)
	{
		DetailsWindowSwitcher->SetActiveWidget(FindGamesMenu);
		FindGamesMenu->RefreshServerList();
	}
}


void UMainMenu::OnErrorPopupButtonClicked()
{
	if(ErrorPopupUI != nullptr)
		ErrorPopupUI->SetVisibility(ESlateVisibility::Hidden);
}


void UMainMenu::ShowErrorMessage(FString ErrorMessage)
{
	if(LoadingPopupUI != nullptr)
		LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);

	if(ErrorPopupUI != nullptr)
	{
		ErrorPopupUI->SetVisibility(ESlateVisibility::Visible);
		
		if(ErrorPopupText != nullptr)
			ErrorPopupText->SetText(FText::FromString(ErrorMessage));
	}
}


void UMainMenu::ToggleLoadingMessage(bool bVisible, FString LoadingMessage)
{
	if(bVisible)
	{
		if(LoadingPopupUI != nullptr)
		{
			LoadingPopupUI->SetVisibility(ESlateVisibility::Visible);

			if(LoadingPopupText != nullptr)
				LoadingPopupText->SetText(FText::FromString(LoadingMessage));
		}
	}
	else
	{
		if(LoadingPopupUI != nullptr)
			LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);
	}
}