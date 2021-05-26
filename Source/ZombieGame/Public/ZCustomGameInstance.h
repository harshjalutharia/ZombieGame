// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "ZombieGame/ZEnums.h"
#include "ZCustomGameInstance.generated.h"

USTRUCT()
struct FCustomGameMode
{
	GENERATED_BODY()

	FString Name;
	FString Description;
	FString URL;
};

USTRUCT()
struct FGameMap
{
	GENERATED_BODY()

	FString Name;
	FString Description;
	FString URL;

	UPROPERTY()
	UTexture2D* Icon;
};

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

	UFUNCTION(BlueprintCallable)
    void LoadLoadingScreen();

	UFUNCTION(BlueprintCallable)
    void LoadPauseMenu();

	/*
	Public functions to load menus
	*/

private:

	UPROPERTY()
	class UMenuWidget* PauseMenu;

	/*
	Interface Functions
	*/

	virtual void Host(FString ServerName, uint8 GameModeIndex, uint8 MapIndex, uint8 MaxPlayers, bool bLanMatch) override;

	virtual void Join(uint32 ServerIndex) override;

	virtual void RefreshServerList() override;

	virtual void CancelServerSearch() override;

	virtual void ShowPauseMenu_Implementation() override;

	virtual void GetAllGameModesAndMaps(TArray<FString>& InAllGameModes, TArray<FString>& InAllMaps) override;

	virtual uint8 GetMaxAllowedPlayers() override;

	virtual void SetFindGamesMenu(UFindGamesMenu* InFindGamesMenu) override;

	/*
	Menu classes
	*/

	UPROPERTY()
	class UMainMenu* MainMenu;

	UPROPERTY()
	class UFindGamesMenu* FindGamesMenu;

	TSubclassOf<UUserWidget> PlayerHUDClass;
	TSubclassOf<UUserWidget> MainMenuClass;
	TSubclassOf<UUserWidget> LoadingScreenClass;
	TSubclassOf<UUserWidget> PauseMenuClass;

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

	/*
	Helper functions
	*/

private:

	void TriggerError(FString ErrorMessage);

	void TriggerLoadingPopup(bool bShowPopup, FString Message = "");

	void DestroySessionCaller();
};