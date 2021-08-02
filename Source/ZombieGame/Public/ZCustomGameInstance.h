// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "Structs/ServerSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "ZCustomGameInstance.generated.h"

#define DEFAULT_MOUSE_SENSITIVITY 50
#define MAX_MOUSE_SENSITIVITY 100
#define MIN_MOUSE_SENSITIVITY 1


/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UZCustomGameInstance : public UGameInstance, public IZINT_GameInstance
{
	GENERATED_BODY()

public:
	
	UZCustomGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init() override;
	
	/*
	Public functions to load menus from level blueprint
	*/

	UFUNCTION(BlueprintCallable)
	void LoadPlayerHUD();

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	/*
	Interface Functions
	*/

	virtual void Host(FString& ServerName, FString& Password, uint8 GameModeIndex, uint8 MapIndex, uint8 MaxPlayers, bool bLanMatch) override;

	virtual void Join(uint32 ServerIndex) override;

	virtual void RefreshServerList() override;

	virtual void CancelServerSearch() override;

	virtual class UPauseMenu* LoadPauseMenu_Implementation() override;

	virtual class ULobbyMenu* LoadLobbyMenu_Implementation() override;

	virtual void GetAllGameModesAndMaps(TArray<FString>& InAllGameModes, TArray<FString>& InAllMaps) const override;

	virtual void GetAllTimeLimitsAndScoreLimits(TArray<FString> &InAllTimeLimits, TArray<FString> &InAllScoreLimits) const override;

	virtual uint8 GetMaxAllowedPlayers() const override;

	virtual void SetFindGamesMenu(class UFindGamesMenu* InFindGamesMenu) override;

	virtual void GetLobbyServerInfo_Implementation(FLobbyServerInfo& OutLobbyServerInfo) const override { OutLobbyServerInfo = LobbyServerInfo; }

	virtual float GetMaxMouseSensitivity() const override { return MAX_MOUSE_SENSITIVITY; }

	virtual float GetMinMouseSensitivity() const override { return MIN_MOUSE_SENSITIVITY; }

	virtual FGameplaySettings& GetGameplaySettings() override;// { return GameplaySettings; }

	virtual void SaveGameplaySettings(FGameplaySettings& NewGameplaySettings) override;

	virtual void GetCurrentLobbyInfoIndexes(uint8& OutGameModeIndex, uint8& OutMapIndex, uint8& OutScoreLimitIndex, uint8& OutTimeLimitIndex) override;

	virtual void HostUpdateLobbyServerInfo(uint8 GameModeIndex, uint8 MapIndex, uint8 ScoreLimitIndex, uint8 TimeLimitIndex) override;

	virtual void GetPlayerInfo_Implementation(FLobbyPlayerInfo& OutPlayerInfo) override;

	virtual void StartGame() override;

	/*
	Menu classes
	*/

private:

	UPROPERTY()
	class UMainMenu* MainMenu;

	UPROPERTY()
	class UPlayerUI* PlayerUI;

	UPROPERTY()
	class UFindGamesMenu* FindGamesMenu;

	UPROPERTY()
	class ULobbyMenu* LobbyMenu;

	UPROPERTY()
	class UPauseMenu* PauseMenu;

	UPROPERTY()
	class ULoadingScreen* LoadingScreen;

	TSubclassOf<UUserWidget> MainMenuClass;
	TSubclassOf<UUserWidget> PlayerUIClass;
	TSubclassOf<UUserWidget> PauseMenuClass;
	TSubclassOf<UUserWidget> LoadingScreenClass;
	TSubclassOf<UUserWidget> LobbyMenuClass;

	/*
	Delegate Handles
	*/

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	
	/*
	Delegate functions
	*/

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool Success);

	/*
	Variables
	*/

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;

	bool bFindingSessions;

	TArray<FCustomGameMode> AllGameModes;
	TArray<FGameMap> AllMaps;
	TArray<FTimeLimit> AllTimeLimits;
	TArray<FScoreLimit> AllScoreLimits;

	uint8 SelectedGameModeIndex;
	uint8 SelectedMapIndex;
	uint8 SelectedTimeLimitIndex;
	uint8 SelectedScoreLimitIndex;

	/*
	Helper functions
	*/

	void HandleNetworkFailure( UWorld* InWorld, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	void HandleTravelFailure( UWorld* InWorld, ETravelFailure::Type FailureType, const FString& ErrorString);

public:

	void TriggerError(FString ErrorMessage);

	void TriggerLoadingPopup(bool bShowPopup, FString Message = "");

	void DestroySessionCaller();

	/*
	*Player info
	*/

protected:

	UPROPERTY(BlueprintReadWrite)
	FLobbyPlayerInfo LobbyPlayerInfo;

	UFUNCTION(BlueprintImplementableEvent)
	void LoadPlayerInfoFromSubsystem();

	/*
	 *Saving and Loading Settings
	 */

private:
	
	FGameplaySettings GameplaySettings;

	void SaveGameplaySettingsToFile();
	void LoadGameplaySettingsFromFile();
	
	/*
	 *Lobby Variables
	 */

	FLobbyServerInfo LobbyServerInfo;

	/*
	 *Lobby Helper Functions
	 */

	void SetLobbyInfo(FString& InServerName, FString& InPassword, uint8 InGameMapIndex, uint8 InCustomGameModeIndex, uint8 InMaxPlayers);

	/*
	 *Loading Screen Functions
	 */

public:

	void ShowLoadingScreen();
	void HideLoadingScreen();

	/*
	 *Show errors after disconnect
	 */

private:

	bool bShowErrorAfterMainMenuLoad;
	FString MainMenuError;
	
public:
	
	void ShowErrorAfterMenuLoaded(const FString& Error);
	
};