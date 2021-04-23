// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"


bool UMainMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(HostButton!=nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostClicked);

	if(!ensure(JoinButton!=nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinClicked);

	if(!ensure(QuitButton!=nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::ExitGame);
	
	if (!ensure(BackButtonHostMenu != nullptr)) return false;
	BackButtonHostMenu->OnClicked.AddDynamic(this, &UMainMenu::OnBackClicked);
	
	if (!ensure(HostButtonHostMenu != nullptr)) return false;
    HostButtonHostMenu->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(BackButtonJoinMenu != nullptr)) return false;
	BackButtonJoinMenu->OnClicked.AddDynamic(this, &UMainMenu::OnBackClicked);

	return true;
}


void UMainMenu::OnHostClicked()
{
	if(MenuSwitcher!=nullptr && HostMenu!=nullptr)
		MenuSwitcher->SetActiveWidget(HostMenu);
}


void UMainMenu::OnJoinClicked()
{
	if(MenuSwitcher!=nullptr && JoinMenu!=nullptr)
		MenuSwitcher->SetActiveWidget(JoinMenu);
}


void UMainMenu::ExitGame()
{
	auto PC = GetOwningPlayer();
	if(PC!=nullptr)
		PC->ConsoleCommand("Quit");
}


void UMainMenu::OnBackClicked()
{
	if(MenuSwitcher!=nullptr && MainMenu!=nullptr)
		MenuSwitcher->SetActiveWidget(MainMenu);
}


void UMainMenu::HostServer()
{
	
}

