// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Interfaces/ZINT_GameInstance.h"


bool UMainMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(LoginButton!=nullptr)) return false;
	LoginButton->OnClicked.AddDynamic(this, &UMainMenu::OnLoginClicked);

	if(!ensure(LoginErrorButton!=nullptr)) return false;
	LoginErrorButton->OnClicked.AddDynamic(this, &UMainMenu::OnLoginErrorClicked);

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

	if(LoginPopup != nullptr)
		LoginPopup->SetVisibility(ESlateVisibility::Hidden);

	return true;
}


void UMainMenu::OnLoginClicked()
{
	if(GameInstanceInterface != nullptr)
	{
		GameInstanceInterface->AttemptLogin();

		if(LoginButton != nullptr)
			LoginButton->SetIsEnabled(false);
		if(LoginPopup != nullptr)
			LoginPopup->SetVisibility(ESlateVisibility::Visible);
		if(LoginAttemptUI != nullptr)
			LoginAttemptUI->SetVisibility(ESlateVisibility::HitTestInvisible);
		if(LoginErrorUI != nullptr)
			LoginErrorUI->SetVisibility(ESlateVisibility::Hidden);
	}
}


void UMainMenu::OnLoginErrorClicked()
{
	if(LoginButton != nullptr)
		LoginButton->SetIsEnabled(true);

	if(LoginPopup != nullptr)
		LoginPopup->SetVisibility(ESlateVisibility::Hidden);
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


void UMainMenu::LoginComplete(bool bWasSuccessful, FString ErrorReason)
{
	if(bWasSuccessful)
	{
		if(MenuSwitcher!=nullptr && MainMenu!=nullptr)
			MenuSwitcher->SetActiveWidget(MainMenu);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LOGIN FAILED: %s"), *ErrorReason);
		
		if(LoginAttemptUI != nullptr)
			LoginAttemptUI->SetVisibility(ESlateVisibility::Hidden);
		if(LoginErrorUI != nullptr)
			LoginErrorUI->SetVisibility(ESlateVisibility::Visible);
	}
}

