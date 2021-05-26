// Fill out your copyright notice in the Description page of Project Settings.


#include "ZCustomGameInstance.h"
#include "MenuSystem/MenuWidget.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/FindGamesMenu.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerUI.h"
#include "Interfaces/ZINT_ZPlayerController.h"


const static FName SESSION_NAME = TEXT("MyLocalSessionName");
const static FName SESSION_SETTINGS_NAME_KEY = TEXT("ServerName");
const static FName SESSION_SETTINGS_SEARCH_KEY = TEXT("CustomSessionSetting");
const static FString SESSION_SETTINGS_SEARCH_VALUE = TEXT("CustomSettingValue");
#define MAX_ALLOWED_PLAYERS 8


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
	
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UZCustomGameInstance::OnCreateSessionComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UZCustomGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UZCustomGameInstance::OnJoinSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UZCustomGameInstance::OnDestroySessionComplete);

	bFindingSessions = false;

	AllGameModes.Add({"Free For All", "All players fight for themselves", ""});
	AllGameModes.Add({"Team DeathMatch", "Players divided into 2 teams", ""});
	AllGameModes.Add({"Search & Destroy", "Team based bomb defuse mode", ""});

	AllMaps.Add({"Factory", "Factory Description", "", nullptr});
	AllMaps.Add({"City", "City Description", "", nullptr});
	AllMaps.Add({"Castle", "Castle Description", "", nullptr});
}


void UZCustomGameInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem != nullptr)
	{
		auto Inter = Subsystem->GetPartyInterface();
		SessionInterface = Subsystem->GetSessionInterface();
		UE_LOG(LogTemp, Warning, TEXT("Using Subsystem: %s"), *Subsystem->GetSubsystemName().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get Online Subsystem"));
	}
	
	Super::Init();
}


void UZCustomGameInstance::LoadPlayerHUD()
{
	if(!PlayerHUDClass) return;
	
	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return;

	UPlayerUI* PlayerHUD = CreateWidget<UPlayerUI>(PC,PlayerHUDClass);
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

	MainMenu->Setup(this);
	MainMenu->ShowMenu(true);

	DestroySessionCaller();
}


void UZCustomGameInstance::LoadLoadingScreen()
{
	if(!LoadingScreenClass) return;

	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return;

	UMenuWidget* LoadingScreen = CreateWidget<UMenuWidget>(PC,LoadingScreenClass);
	if(!ensure(LoadingScreen!=nullptr)) return;

	LoadingScreen->Setup(this);
	LoadingScreen->ShowMenu(false);
}


void UZCustomGameInstance::LoadPauseMenu()
{
	if(PauseMenu != nullptr)
		return;
	
	if(!PauseMenuClass) return;

	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return;

	PauseMenu = CreateWidget<UMenuWidget>(PC,PauseMenuClass);
	if(!ensure(PauseMenu!=nullptr)) return;

	PauseMenu->Setup(this);
	PauseMenu->ShowMenu(true);
}


void UZCustomGameInstance::Host(FString ServerName, uint8 GameModeIndex, uint8 MapIndex, uint8 MaxPlayers, bool bLanMatch)
{
	if(SessionInterface.IsValid())
	{
		if(bFindingSessions)
			CancelServerSearch();
		
		FOnlineSessionSettings SessionSettings;

		SessionSettings.NumPublicConnections = MaxPlayers;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bIsLANMatch = bLanMatch;

		/*if(IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
			SessionSettings.bIsLANMatch = true;
		else
			SessionSettings.bIsLANMatch = false;*/
		
		SessionSettings.Set(SESSION_SETTINGS_SEARCH_KEY, SESSION_SETTINGS_SEARCH_VALUE, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(SESSION_SETTINGS_NAME_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
		if(!SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings))
		{
			TriggerError("Error creating server");
		}
		else
		{
			TriggerLoadingPopup(true, "Creating Server");
		}
	}
	else
	{
		TriggerError("No OSS found");
	}
}


void UZCustomGameInstance::Join(uint32 ServerIndex)
{
	if(SessionInterface.IsValid())
	{
		if(bFindingSessions)
			CancelServerSearch();
		JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
		if(!SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[ServerIndex]))
		{
			TriggerError("Error joining server");
		}
		else
		{
			TriggerLoadingPopup(true, "Joining Server");
		}
	}
	else
	{
		TriggerError("No OSS found");
	}
}


void UZCustomGameInstance::RefreshServerList()
{
	if(SessionInterface.IsValid())
	{
		if(!bFindingSessions)
		{
			bFindingSessions = true;
		
			SessionSearch = MakeShared<FOnlineSessionSearch>();
			SessionSearch->QuerySettings.SearchParams.Empty();
			SessionSearch->MaxSearchResults = 100;
			SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
			SessionSearch->QuerySettings.Set(SESSION_SETTINGS_SEARCH_KEY, SESSION_SETTINGS_SEARCH_VALUE, EOnlineComparisonOp::Equals);

			FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			if(!SessionInterface->FindSessions(0,SessionSearch.ToSharedRef()))
			{
				bFindingSessions = false;
				TriggerError("Error finding servers");
			}
			else if(FindGamesMenu != nullptr)
			{
				FindGamesMenu->ToggleLoadingPopupUI(true);
			}
		}
	}
	else
	{
		TriggerError("No OSS found");
	}
}


void UZCustomGameInstance::CancelServerSearch()
{
	if(SessionInterface.IsValid())
	{
		SessionInterface->CancelFindSessions();
		bFindingSessions = false;

		if(FindGamesMenu != nullptr)
			FindGamesMenu->ToggleLoadingPopupUI(false);
	}
}


void UZCustomGameInstance::ShowPauseMenu_Implementation()
{
	if(PauseMenu != nullptr)
	{
		PauseMenu->ShowMenu();
	}
	else
	{
		LoadPauseMenu();
	}
}


void UZCustomGameInstance::GetAllGameModesAndMaps(TArray<FString>& InAllGameModes, TArray<FString>& InAllMaps)
{
	InAllGameModes.Empty();
	InAllMaps.Empty();

	for(auto& Mode : AllGameModes)
	{
		InAllGameModes.Add(Mode.Name);
	}

	for(auto& Map : AllMaps)
	{
		InAllMaps.Add(Map.Name);
	}
}


uint8 UZCustomGameInstance::GetMaxAllowedPlayers()
{
	return MAX_ALLOWED_PLAYERS;
}


void UZCustomGameInstance::SetFindGamesMenu(UFindGamesMenu* InFindGamesMenu)
{
	FindGamesMenu = InFindGamesMenu;
}


void UZCustomGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if(!Success)
	{
		TriggerError("Failed to create server");
	}
	else
	{
		TriggerLoadingPopup(false);
		
		UWorld* World = GetWorld();
		if(!ensure(World != nullptr)) return;

		World->ServerTravel("/Game/Levels/TestMap?listen");
	}
	
	if(SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
}


void UZCustomGameInstance::OnFindSessionsComplete(bool Success)
{
	if(FindGamesMenu != nullptr)
		FindGamesMenu->ToggleLoadingPopupUI(false);
	
	bFindingSessions = false;
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
		if(FindGamesMenu != nullptr)
		{
			FindGamesMenu->SetServerList(AllServerData);
		}
	}
	else
	{
		if(FindGamesMenu != nullptr)
		{
			FindGamesMenu->ClearServerList();
		}
		
	}

	if(SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}
}


void UZCustomGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		if(SessionInterface.IsValid())
		{
			FString ConnectInfo;
			if(SessionInterface->GetResolvedConnectString(SESSION_NAME, ConnectInfo))
			{
				TriggerLoadingPopup(false);
				
				APlayerController* PC = GetFirstLocalPlayerController();
				if(PC != nullptr)
					PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
			}
			else
			{
				TriggerError("Failed to resolve connection info");
			}
		}
	}
	else
	{
		TriggerError("Failed to join server");
	}

	if(SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
}


void UZCustomGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if(!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Destroy Session (Call completed)"));
	}
}


void UZCustomGameInstance::TriggerError(FString ErrorMessage)
{
	if(MainMenu != nullptr)
	{
		MainMenu->ShowErrorMessage(ErrorMessage);
	}
}


void UZCustomGameInstance::TriggerLoadingPopup(bool bShowPopup, FString Message)
{
	if(MainMenu != nullptr)
	{
		MainMenu->ToggleLoadingMessage(bShowPopup,Message);
	}
}


void UZCustomGameInstance::DestroySessionCaller()
{
	if(SessionInterface->GetNamedSession(SESSION_NAME) != nullptr)
		SessionInterface->DestroySession(SESSION_NAME);
}
