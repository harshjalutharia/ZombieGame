// Fill out your copyright notice in the Description page of Project Settings.


#include "ZCustomGameInstance.h"
#include "MenuSystem/MenuWidget.h"
#include "MenuSystem/MainMenu.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/ZINT_ZPlayerController.h"


const static FName SESSION_NAME = TEXT("MyLocalSessionName");
const static FName SESSION_SETTINGS_NAME_KEY = TEXT("Server Name");
const static FName SESSION_SETTINGS_SEARCH_KEY = TEXT("CustomSessionSetting");
const static FString SESSION_SETTINGS_SEARCH_VALUE = TEXT("CustomSettingValue");


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


void UZCustomGameInstance::Host(FString ServerName)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem != nullptr)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{			
			CreateSessionDelegate = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionComplete::FDelegate::CreateUObject(this, &UZCustomGameInstance::OnCreateSessionComplete));
			
			FOnlineSessionSettings SessionSettings;

			SessionSettings.NumPublicConnections = 4;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bUsesPresence = false;
			SessionSettings.bIsLANMatch = true;
			
			//SessionSettings.Set(SESSION_SETTINGS_SEARCH_KEY, SESSION_SETTINGS_SEARCH_VALUE, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			SessionSettings.Settings.Add(SESSION_SETTINGS_NAME_KEY, ServerName);
			SessionSettings.Settings.Add(SESSION_SETTINGS_SEARCH_KEY, SESSION_SETTINGS_SEARCH_VALUE);
			//SessionSettings.Set(SESSION_SETTINGS_NAME_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

			if(!SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings))
			{
				UE_LOG(LogTemp, Warning, TEXT("ERROR CREATING SESSION"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ERROR CREATING SESSION"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR CREATING SESSION"));
	}
}


void UZCustomGameInstance::Join(uint32 ServerIndex)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem != nullptr)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface != nullptr)
		{
			JoinSessionDelegate = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionComplete::FDelegate::CreateUObject(this,&UZCustomGameInstance::OnJoinSessionComplete));

			if(!SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[ServerIndex]))
			{
				UE_LOG(LogTemp, Warning, TEXT("ERROR JOINING SESSION"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ERROR JOINING SESSION"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROR JOINING SESSION"));
	}
}


void UZCustomGameInstance::RefreshServerList()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem != nullptr)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface != nullptr)
		{
			SessionSearch = MakeShared<FOnlineSessionSearch>();
			SessionSearch->QuerySettings.SearchParams.Empty();
			SessionSearch->QuerySettings.Set(SESSION_SETTINGS_SEARCH_KEY, SESSION_SETTINGS_SEARCH_VALUE, EOnlineComparisonOp::Equals);

			FindSessionDelegate = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsComplete::FDelegate::CreateUObject(this, &UZCustomGameInstance::OnFindSessionsComplete, SessionSearch.ToSharedRef()));

			if(!SessionInterface->FindSessions(0,SessionSearch.ToSharedRef()))
			{
				UE_LOG(LogTemp, Warning, TEXT("1ERROR FINDING SESSION"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("2ERROR FINDING SESSION"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("3ERROR FINDING SESSION"));
	}
}


void UZCustomGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if(!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session creation failed"));
	}

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem != nullptr)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegate);
		}
	}
	CreateSessionDelegate.Reset();

	UWorld* World = GetWorld();
	if(!ensure(World != nullptr)) return;

	World->ServerTravel("/Game/Levels/TestMap?listen");
}


void UZCustomGameInstance::OnFindSessionsComplete(bool Success, TSharedRef<FOnlineSessionSearch> SessionSearchResults)
{
	if(Success && SessionSearch->SearchResults.Num() > 0)
	{
		TArray<FServerData> AllServerData;
		for(auto Result : SessionSearch->SearchResults)
		{
			if(Result.IsValid())
			{				
				FServerData ServerInfo;
				ServerInfo.HostUsername = Result.Session.OwningUserName;
				ServerInfo.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
				ServerInfo.CurrentPlayers = ServerInfo.MaxPlayers - Result.Session.NumOpenPublicConnections;

				FString Name;
				if(Result.Session.SessionSettings.Get(SESSION_SETTINGS_NAME_KEY, Name))
				{
					ServerInfo.ServerName = Name;
				}
				else
				{
					ServerInfo.ServerName = "<No Server Name>";
				}

				AllServerData.Add(ServerInfo);
			}
		}
		if(MainMenu != nullptr)
		{
			MainMenu->SetServerList(AllServerData);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("4NO SESSION FOUND"));
		if(MainMenu != nullptr)
		{
			MainMenu->ClearServerList();
		}
	}
}


void UZCustomGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		FString ConnectInfo;
		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
		if(Subsystem != nullptr)
		{
			IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
			if(SessionInterface != nullptr)
			{
				if(SessionInterface->GetResolvedConnectString(SESSION_NAME, ConnectInfo))
				{
					APlayerController* PC = GetFirstLocalPlayerController();
					if(PC != nullptr)
						PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("FAILED TO RESOLVE CONNECT STRING"));
				}
			}
		}
	}
}
