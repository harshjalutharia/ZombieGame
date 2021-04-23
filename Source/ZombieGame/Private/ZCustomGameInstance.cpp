// Fill out your copyright notice in the Description page of Project Settings.


#include "ZCustomGameInstance.h"
#include "MenuSystem/MenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/ZINT_ZPlayerController.h"


UZCustomGameInstance::UZCustomGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget>PlayerHUDWidgetBPClass(TEXT("/Game/UI/WBP_PlayerUI"));
	if(!PlayerHUDWidgetBPClass.Class) return;
	PlayerHUDClass = PlayerHUDWidgetBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget>MainMenuWidgetBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if(!MainMenuWidgetBPClass.Class) return;
	MainMenuClass = MainMenuWidgetBPClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget>LoadingScreenWidgetBPClass(TEXT("/Game/MenuSystem/WBP_LoadingScreen"));
	if(!LoadingScreenWidgetBPClass.Class) return;
	LoadingScreenClass = LoadingScreenWidgetBPClass.Class;
}


void UZCustomGameInstance::LoadPlayerHUD()
{
	if(!PlayerHUDClass) return;
	
	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return;

	UUserWidget* PlayerHUD = CreateWidget(PC,PlayerHUDClass);
	if(!ensure(PlayerHUD!=nullptr)) return;

	PlayerHUD->AddToViewport();

	if(PC->GetClass()->ImplementsInterface(UZINT_ZPlayerController::StaticClass()))
	{
		IZINT_ZPlayerController::Execute_AssignPlayerHUD(PC,PlayerHUD);
	}
}


void UZCustomGameInstance::LoadMainMenu()
{
	if(!MainMenuClass) return;

	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return;

	UMenuWidget* MainMenu = CreateWidget<UMenuWidget>(PC,MainMenuClass);
	if(!ensure(MainMenu!=nullptr)) return;

	MainMenu->Setup(true);
}


void UZCustomGameInstance::LoadLoadingScreen()
{
	if(!LoadingScreenClass) return;

	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return;

	UMenuWidget* LoadingScreen = CreateWidget<UMenuWidget>(PC,LoadingScreenClass);
	if(!ensure(LoadingScreen!=nullptr)) return;

	LoadingScreen->Setup(false);
}
