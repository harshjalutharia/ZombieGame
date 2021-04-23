// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/PauseMenu.h"
#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"


bool UPauseMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(BackButton != nullptr)) return false;
	BackButton->OnClicked.AddDynamic(this, &UPauseMenu::OnBackClicked);

	if (!ensure(ExitButton != nullptr)) return false;
	ExitButton->OnClicked.AddDynamic(this, &UPauseMenu::OnExitClicked);

	return true;
}


void UPauseMenu::OnBackClicked()
{
	Teardown();
}


void UPauseMenu::OnExitClicked()
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

