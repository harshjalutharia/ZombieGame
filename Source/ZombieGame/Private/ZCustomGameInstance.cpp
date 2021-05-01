// Fill out your copyright notice in the Description page of Project Settings.


#include "ZCustomGameInstance.h"
#include "MenuSystem/MenuWidget.h"
#include "MenuSystem/MainMenu.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/ZINT_ZPlayerController.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"


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

	ConstructorHelpers::FClassFinder<UUserWidget>PauseMenuWidgetBPClass(TEXT("/Game/MenuSystem/WBP_PauseMenu"));
	if(!PauseMenuWidgetBPClass.Class) return;
	PauseMenuClass = PauseMenuWidgetBPClass.Class;
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

	MainMenu = CreateWidget<UMainMenu>(PC,MainMenuClass);
	if(!ensure(MainMenu!=nullptr)) return;

	MainMenu->Setup(true, this);
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


void UZCustomGameInstance::LoadPauseMenu()
{
	if(!PauseMenuClass) return;

	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return;

	UMenuWidget* PauseMenu = CreateWidget<UMenuWidget>(PC,PauseMenuClass);
	if(!ensure(PauseMenu!=nullptr)) return;

	PauseMenu->Setup(true, this);
}


void UZCustomGameInstance::AttemptLogin()
{
	UE_LOG(LogTemp, Warning, TEXT("Logging in"));
	IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if(Identity.IsValid())
		{
			if(Identity->GetLoginStatus(0)  == ELoginStatus::LoggedIn)
			{
				if(MainMenu != nullptr)
					MainMenu->LoginComplete(true);
			}
			else
			{
				LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(0,FOnLoginComplete::FDelegate::CreateUObject(this, &UZCustomGameInstance::OnLoginComplete));
				if(!Identity->AutoLogin(0))
				{
					UE_LOG(LogTemp, Warning, TEXT("LOGIN SERVICE FAILED TO START"));
					if(MainMenu != nullptr)
						MainMenu->LoginComplete(false, "SERVICE FAILED TO START");
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LOGIN SERVICE FAILED TO START"));
			if(MainMenu != nullptr)
				MainMenu->LoginComplete(false, "SERVICE FAILED TO START");
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LOGIN SERVICE FAILED TO START"));
		if(MainMenu != nullptr)
			MainMenu->LoginComplete(false, "SERVICE FAILED TO START");
	}
}


void UZCustomGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId &UserId, const FString &Error)
{
	if(bWasSuccessful)
	{
		if(MainMenu != nullptr)
			MainMenu->LoginComplete(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LOGIN ERROR"));
		if(MainMenu != nullptr)
			MainMenu->LoginComplete(false, "ATTEMPT FAILED");
	}

	IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if(Identity.IsValid())
		{
			Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginDelegateHandle);
		}
	}
	LoginDelegateHandle.Reset();
}
