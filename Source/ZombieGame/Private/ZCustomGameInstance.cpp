// Fill out your copyright notice in the Description page of Project Settings.


#include "ZCustomGameInstance.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/FindGamesMenu.h"
#include "MenuSystem/LobbyMenu.h"
#include "MenuSystem/PauseMenu.h"
#include "LobbyGameMode.h"
#include "PlayerCharacter.h"
#include "UI/PlayerUI.h"
#include "Interfaces/ZINT_ZPlayerController.h"
#include "SaveGames/OptionsSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "MenuSystem/LoadingScreen.h"


const static FName SESSION_NAME = TEXT("MyLocalSessionName");
const static FName SESSION_SETTINGS_NAME_KEY = TEXT("ServerNameKey");
const static FName SESSION_SETTINGS_PASSWORD_KEY = TEXT("ServerPasswordKey");
const static FName SESSION_SETTINGS_MAPNAME_KEY = TEXT("ServerMapNameKey");
const static FName SESSION_SETTINGS_GAMEMODE_KEY = TEXT("ServerGameModeKey");
const static FName SESSION_SETTINGS_SEARCH_KEY = TEXT("ServerSearchKey");
const static FString SESSION_SETTINGS_SEARCH_VALUE = TEXT("ServerSearchValue");
const static FString OPTIONS_SAVE_GAME_SLOT_NAME = TEXT("OptionsSaveGameSlot");
#define MAX_ALLOWED_PLAYERS 8


UZCustomGameInstance::UZCustomGameInstance(const FObjectInitializer& ObjectInitializer)
{
	const ConstructorHelpers::FClassFinder<UUserWidget>PlayerUIWidgetBPClass(TEXT("/Game/UI/WBP_PlayerUI"));
	if(!PlayerUIWidgetBPClass.Class) return;
	PlayerUIClass = PlayerUIWidgetBPClass.Class;

	const ConstructorHelpers::FClassFinder<UUserWidget>MainMenuWidgetBPClass(TEXT("/Game/MenuSystem/MainMenu/WBP_MainMenu"));
	if(!MainMenuWidgetBPClass.Class) return;
	MainMenuClass = MainMenuWidgetBPClass.Class;

	const ConstructorHelpers::FClassFinder<UUserWidget>LoadingScreenWidgetBPClass(TEXT("/Game/MenuSystem/General/WBP_LoadingScreen"));
	if(!LoadingScreenWidgetBPClass.Class) return;
	LoadingScreenClass = LoadingScreenWidgetBPClass.Class;

	const ConstructorHelpers::FClassFinder<UUserWidget>PauseMenuWidgetBPClass(TEXT("/Game/MenuSystem/General/WBP_PauseMenu"));
	if(!PauseMenuWidgetBPClass.Class) return;
	PauseMenuClass = PauseMenuWidgetBPClass.Class;

	const ConstructorHelpers::FClassFinder<UUserWidget>LobbyMenuWidgetBPClass(TEXT("/Game/MenuSystem/LobbyMenu/WBP_LobbyMenu"));
	if(!LobbyMenuWidgetBPClass.Class) return;
	LobbyMenuClass = LobbyMenuWidgetBPClass.Class;
	
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UZCustomGameInstance::OnCreateSessionComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UZCustomGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UZCustomGameInstance::OnJoinSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UZCustomGameInstance::OnDestroySessionComplete);

	bFindingSessions = false;

	AllGameModes.Add({"Free For All", "All players fight for themselves", "BP_GMFreeForAll.BP_GMFreeForAll_C"});
	AllGameModes.Add({"Team DeathMatch", "Players divided into 2 teams", "BP_GMTeamDeathMatch.BP_GMTeamDeathMatch_C"});
	AllGameModes.Add({"Domination", "Team based bomb defuse mode", "BP_GMDomination.BP_GMDomination_C"});

	AllMaps.Add({"Army Base", "Factory Description", "ArmyBase", nullptr});
	AllMaps.Add({"Desert City", "City Description", "DesertCity", nullptr});
	AllMaps.Add({"Office", "Castle Description", "Office", nullptr});
	AllMaps.Add({"Village", "Castle Description", "Village", nullptr});
	
	AllTimeLimits.Add({"05:00", 300});
	AllTimeLimits.Add({"10:00",600});
	AllTimeLimits.Add({"15:00",900});
	AllTimeLimits.Add({"20:00", 1200});
	AllTimeLimits.Add({"25:00",1500});
	AllTimeLimits.Add({"30:00",1800});
	AllTimeLimits.Add({"35:00",2100});
	AllTimeLimits.Add({"40:00",2400});
	AllTimeLimits.Add({"45:00", 2700});
	AllTimeLimits.Add({"50:00",3000});
	AllTimeLimits.Add({"55:00",3300});

	AllScoreLimits.Add({"25",25});
	AllScoreLimits.Add({"50",50});
	AllScoreLimits.Add({"75",75});
	AllScoreLimits.Add({"100",100});
	AllScoreLimits.Add({"125",125});
	AllScoreLimits.Add({"150",150});
	AllScoreLimits.Add({"175",175});
	AllScoreLimits.Add({"200",200});
	AllScoreLimits.Add({"225",225});
	AllScoreLimits.Add({"250",250});
	AllScoreLimits.Add({"275",275});
	AllScoreLimits.Add({"300",300});
	AllScoreLimits.Add({"325",325});
	AllScoreLimits.Add({"350",350});
	AllScoreLimits.Add({"375",375});
	AllScoreLimits.Add({"400",400});
	AllScoreLimits.Add({"425",425});
	AllScoreLimits.Add({"450",450});
	AllScoreLimits.Add({"475",475});
	AllScoreLimits.Add({"500",500});
}


void UZCustomGameInstance::Init()
{
	Super::Init();
	
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

	LoadGameplaySettingsFromFile();
	
	GetEngine()->OnNetworkFailure().AddUObject(this, &UZCustomGameInstance::HandleNetworkFailure);
	GetEngine()->OnTravelFailure().AddUObject(this, &UZCustomGameInstance::HandleTravelFailure);
}


void UZCustomGameInstance::LoadPlayerHUD()
{
	if(!PlayerUIClass) return;

	if(PlayerUI != nullptr)
		PlayerUI->RemoveFromParent();

	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return;

	PlayerUI = CreateWidget<UPlayerUI>(PC,PlayerUIClass);
	if(!ensure(PlayerUI!=nullptr)) return;

	HideLoadingScreen();

	PlayerUI->AddToViewport();

	if(PC->GetClass()->ImplementsInterface(UZINT_ZPlayerController::StaticClass()))
	{
		IZINT_ZPlayerController::Execute_AssignPlayerHUD(PC,PlayerUI);
	}
}


void UZCustomGameInstance::LoadMainMenu()
{
	if(!MainMenuClass) return;

	if(MainMenu != nullptr)
	{
		MainMenu->RemoveFromParent();
	}

	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return;

	MainMenu = CreateWidget<UMainMenu>(PC,MainMenuClass);
	if(!ensure(MainMenu!=nullptr)) return;

	MainMenu->Setup(this);
	MainMenu->ShowMenu(true);

	DestroySessionCaller();
	
	LoadPlayerInfoFromSubsystem();
}


void UZCustomGameInstance::Host(FString& ServerName, FString& Password, uint8 GameModeIndex, uint8 MapIndex, uint8 MaxPlayers, bool bLanMatch)
{
	if(SessionInterface.IsValid())
	{
		if(bFindingSessions)
			CancelServerSearch();

		SetLobbyInfo(ServerName, Password, MapIndex, GameModeIndex, MaxPlayers);

		FOnlineSessionSettings SessionSettings;

		SessionSettings.NumPublicConnections = MaxPlayers;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bIsLANMatch = bLanMatch;
		
		SessionSettings.Set(SESSION_SETTINGS_SEARCH_KEY, SESSION_SETTINGS_SEARCH_VALUE, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(SESSION_SETTINGS_NAME_KEY, LobbyServerInfo.ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(SESSION_SETTINGS_PASSWORD_KEY, LobbyServerInfo.Password, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings.Set(SESSION_SETTINGS_MAPNAME_KEY, LobbyServerInfo.MapName, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings.Set(SESSION_SETTINGS_GAMEMODE_KEY, LobbyServerInfo.GameModeName, EOnlineDataAdvertisementType::ViaOnlineService);

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


UPauseMenu* UZCustomGameInstance::LoadPauseMenu_Implementation()
{
	if(!PauseMenuClass)
		return nullptr;

	if(PauseMenu != nullptr)
		return PauseMenu;

	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return nullptr;

	PauseMenu = CreateWidget<UPauseMenu>(PC,PauseMenuClass);
	if(!ensure(PauseMenu!=nullptr)) return nullptr;

	PauseMenu->Setup(this);

	return PauseMenu;
}


ULobbyMenu* UZCustomGameInstance::LoadLobbyMenu_Implementation()
{
	if(!LobbyMenuClass) return nullptr;

	if(LobbyMenu != nullptr)
	{
		LobbyMenu->RemoveFromParent();
	}

	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!ensure(PC!=nullptr)) return nullptr;
	
	LobbyMenu = CreateWidget<ULobbyMenu>(PC,LobbyMenuClass);
	if(!ensure(LobbyMenu!=nullptr)) return nullptr;

	LobbyMenu->Setup(this);
	LobbyMenu->ShowMenu(true);

	return LobbyMenu;
}


void UZCustomGameInstance::GetAllGameModesAndMaps(TArray<FString>& InAllGameModes, TArray<FString>& InAllMaps) const
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


void UZCustomGameInstance::GetAllTimeLimitsAndScoreLimits(TArray<FString>& InAllTimeLimits,
	TArray<FString>& InAllScoreLimits) const
{
	InAllTimeLimits.Empty();
	InAllScoreLimits.Empty();

	for(auto& TimeLimit : AllTimeLimits)
	{
		InAllTimeLimits.Add(TimeLimit.TimeLimitString);
	}
	
	for(auto& ScoreLimit : AllScoreLimits)
	{
		InAllScoreLimits.Add(ScoreLimit.ScoreLimitString);
	}
}


uint8 UZCustomGameInstance::GetMaxAllowedPlayers() const
{
	return MAX_ALLOWED_PLAYERS;
}


void UZCustomGameInstance::SetFindGamesMenu(UFindGamesMenu* InFindGamesMenu)
{
	FindGamesMenu = InFindGamesMenu;
}


FGameplaySettings& UZCustomGameInstance::GetGameplaySettings()
{
	LoadGameplaySettingsFromFile();
	return GameplaySettings;
}


void UZCustomGameInstance::SaveGameplaySettings(FGameplaySettings& NewGameplaySettings)
{
	if(GameplaySettings != NewGameplaySettings)
	{
		GameplaySettings = NewGameplaySettings;
		SaveGameplaySettingsToFile();

		if(APlayerCharacter* ControlledPlayerCharacter = Cast<APlayerCharacter>(GetFirstLocalPlayerController()->GetPawn()))
			ControlledPlayerCharacter->SetGameplaySettings(GameplaySettings);
	}
}


void UZCustomGameInstance::GetCurrentLobbyInfoIndexes(uint8& OutGameModeIndex, uint8& OutMapIndex, uint8& OutScoreLimitIndex,
	uint8& OutTimeLimitIndex)
{
	OutGameModeIndex = SelectedGameModeIndex;
	OutMapIndex = SelectedMapIndex;
	OutScoreLimitIndex = SelectedScoreLimitIndex;
	OutTimeLimitIndex = SelectedTimeLimitIndex;
}


void UZCustomGameInstance::HostUpdateLobbyServerInfo(uint8 GameModeIndex, uint8 MapIndex, uint8 ScoreLimitIndex,
													uint8 TimeLimitIndex)
{
	LobbyServerInfo.MapName = AllMaps[MapIndex].Name;
	LobbyServerInfo.GameModeName = AllGameModes[GameModeIndex].Name;
	LobbyServerInfo.TimeLimit = AllTimeLimits[TimeLimitIndex].TimeLimitString;
	LobbyServerInfo.ScoreLimit = AllScoreLimits[ScoreLimitIndex].ScoreLimitString;

	SelectedMapIndex = MapIndex;
	SelectedGameModeIndex = GameModeIndex;
	SelectedScoreLimitIndex = ScoreLimitIndex;
	SelectedTimeLimitIndex = TimeLimitIndex;
	
	ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	LobbyGameMode->UpdateLobbyServerInfoOnClients(LobbyServerInfo);

	if(SessionInterface != nullptr)
	{
		auto SessionSettings = SessionInterface->GetSessionSettings(SESSION_NAME);
		SessionSettings->Remove(SESSION_SETTINGS_MAPNAME_KEY);
		SessionSettings->Remove(SESSION_SETTINGS_GAMEMODE_KEY);

		SessionSettings->Set(SESSION_SETTINGS_MAPNAME_KEY, LobbyServerInfo.MapName, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionSettings->Set(SESSION_SETTINGS_GAMEMODE_KEY, LobbyServerInfo.GameModeName, EOnlineDataAdvertisementType::ViaOnlineService);

		SessionInterface->UpdateSession(SESSION_NAME,*SessionSettings,true);
	}
}


void UZCustomGameInstance::GetPlayerInfo_Implementation(FLobbyPlayerInfo& OutPlayerInfo)
{
	OutPlayerInfo = LobbyPlayerInfo;
}


void UZCustomGameInstance::StartGame()
{
	UWorld* World = GetWorld();
	if(!ensure(World != nullptr)) return;
	
	World->ServerTravel(FString::Printf(TEXT("/Game/Levels/Maps/%s?game=/Game/Blueprints/%s"),*AllMaps[SelectedMapIndex].URL,*AllGameModes[SelectedGameModeIndex].URL));
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

		World->ServerTravel("/Game/Levels/LobbyLevel?listen");
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
		TArray<FLobbyServerInfo> AllServerInfo;
		for(auto Result : SessionSearch->SearchResults)
		{
			if(Result.IsValid())
			{				
				FLobbyServerInfo ServerInfo;
				ServerInfo.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
				ServerInfo.CurrentPlayers = ServerInfo.MaxPlayers - Result.Session.NumOpenPublicConnections;

				FString ServerName, MapName, GameModeName, Password;
				
				if(Result.Session.SessionSettings.Get(SESSION_SETTINGS_NAME_KEY, ServerName))
					ServerInfo.ServerName = ServerName;
				else
					ServerInfo.ServerName = "<No Server Name>";

				if(Result.Session.SessionSettings.Get(SESSION_SETTINGS_PASSWORD_KEY, Password))
					ServerInfo.Password = Password;
				else
					ServerInfo.Password = "";
				
				if(Result.Session.SessionSettings.Get(SESSION_SETTINGS_GAMEMODE_KEY, GameModeName))
					ServerInfo.GameModeName = GameModeName;
				else
					ServerInfo.GameModeName = "<No GameMode Name>";
				
				if(Result.Session.SessionSettings.Get(SESSION_SETTINGS_MAPNAME_KEY, MapName))
					ServerInfo.MapName = MapName;
				else
					ServerInfo.MapName = "<No Map Name>";

				AllServerInfo.Add(ServerInfo);
			}
		}
		if(FindGamesMenu != nullptr)
		{
			FindGamesMenu->SetServerList(AllServerInfo);
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


void UZCustomGameInstance::HandleNetworkFailure(UWorld* InWorld, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	TriggerError(FString::Printf(TEXT("Network Failure: %s"),*ErrorString));
	UE_LOG(LogTemp, Warning, TEXT("Network Failure: %s"),*ErrorString);
}


void UZCustomGameInstance::HandleTravelFailure(UWorld* InWorld, ETravelFailure::Type FailureType,
	const FString& ErrorString)
{
	TriggerError(FString::Printf(TEXT("Travel Failure: %s"),*ErrorString));
	UE_LOG(LogTemp, Warning, TEXT("Travel Failure: %s"),*ErrorString);
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


void UZCustomGameInstance::SaveGameplaySettingsToFile()
{
	/*if(UGameplayStatics::DoesSaveGameExist(OPTIONS_SAVE_GAME_SLOT_NAME,0))
	{
		OptionsSaveGame = Cast<UOptionsSaveGame>(UGameplayStatics::LoadGameFromSlot(OPTIONS_SAVE_GAME_SLOT_NAME,0));
		if(OptionsSaveGame != nullptr)
		{
			OptionsSaveGame->GameplaySettings = GameplaySettings;
		}
	}*/
	if(UOptionsSaveGame* ExistingOptionsSaveGame = Cast<UOptionsSaveGame>(UGameplayStatics::LoadGameFromSlot(OPTIONS_SAVE_GAME_SLOT_NAME,0)))
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveMouseSens existing"))
		
		ExistingOptionsSaveGame->GameplaySettings = GameplaySettings;
		UGameplayStatics::SaveGameToSlot(ExistingOptionsSaveGame,OPTIONS_SAVE_GAME_SLOT_NAME,0);
		UE_LOG(LogTemp, Warning, TEXT("SaveFileMouseSens: %f"),ExistingOptionsSaveGame->GameplaySettings.MouseSensitivity);
	}

	else
	{
		if(UOptionsSaveGame* NewSave = Cast<UOptionsSaveGame>(UGameplayStatics::CreateSaveGameObject(UOptionsSaveGame::StaticClass())))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveMouseSens new"))
			NewSave->GameplaySettings = GameplaySettings;
			UGameplayStatics::SaveGameToSlot(NewSave,OPTIONS_SAVE_GAME_SLOT_NAME,0);
		}
	}
}


void UZCustomGameInstance::LoadGameplaySettingsFromFile()
{
	if(UOptionsSaveGame* OldSave = Cast<UOptionsSaveGame>(UGameplayStatics::LoadGameFromSlot(OPTIONS_SAVE_GAME_SLOT_NAME,0)))
	{
		GameplaySettings.MouseSensitivity = OldSave->GameplaySettings.MouseSensitivity;
		if(GameplaySettings.MouseSensitivity == 0)
			GameplaySettings.MouseSensitivity = DEFAULT_MOUSE_SENSITIVITY;
		GameplaySettings.bToggleADS = OldSave->GameplaySettings.bToggleADS;
		
		UE_LOG(LogTemp, Warning, TEXT("init moussens save game"))
		UE_LOG(LogTemp, Warning, TEXT("SaveFileMouseSens: %f"),OldSave->GameplaySettings.MouseSensitivity);
	}
	else
	{
		GameplaySettings.MouseSensitivity = DEFAULT_MOUSE_SENSITIVITY;
		GameplaySettings.bToggleADS = false;
		UE_LOG(LogTemp, Warning, TEXT("init moussens default"))
	}
}


void UZCustomGameInstance::SetLobbyInfo(FString& InServerName, FString& InPassword, uint8 InGameMapIndex, uint8 InCustomGameModeIndex, uint8 InMaxPlayers)
{
	LobbyServerInfo.ServerName = InServerName;
	LobbyServerInfo.Password = InPassword;
	LobbyServerInfo.MapName = AllMaps[InGameMapIndex].Name;
	LobbyServerInfo.GameModeName = AllGameModes[InCustomGameModeIndex].Name;
	LobbyServerInfo.CurrentPlayers = 0;
	LobbyServerInfo.MaxPlayers = InMaxPlayers;
	LobbyServerInfo.ScoreLimit = AllScoreLimits[0].ScoreLimitString;
	LobbyServerInfo.TimeLimit = AllTimeLimits[0].TimeLimitString;

	SelectedMapIndex = InGameMapIndex;
	SelectedGameModeIndex = InCustomGameModeIndex;
	SelectedScoreLimitIndex = 0;
	SelectedTimeLimitIndex = 0;
}


void UZCustomGameInstance::ShowLoadingScreen()
{
	if(LoadingScreen==nullptr)
	{
		LoadingScreen = CreateWidget<ULoadingScreen>(this,LoadingScreenClass);
		if(!ensure(LoadingScreen!=nullptr)) return;
	}
	
	GetGameViewportClient()->AddViewportWidgetContent(LoadingScreen->TakeWidget(),10);
	LoadingScreen->PlayFadeInAnimation();
}


void UZCustomGameInstance::HideLoadingScreen()
{
	if(LoadingScreen != nullptr)
		LoadingScreen->PlayFadeOutAnimation();
}
