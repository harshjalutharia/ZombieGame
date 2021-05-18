// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "ZCustomGameInstance.generated.h"

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

	class IZINT_ZPlayerController* PlayerControllerInterface;

public:

	void ShowPauseMenu();

	void SetPlayerControllerInterface(class IZINT_ZPlayerController* InPlayerControllerInterface);

	/*
	Interface Functions
	*/

	virtual void Host(FString ServerName) override;

	virtual void Join(uint32 ServerIndex) override;

	virtual void RefreshServerList() override;

	virtual void CancelServerSearch() override;

private:

	/*
	Menu classes
	*/

	UPROPERTY()
	class UMainMenu* MainMenu;

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

	/*
	Helper functions
	*/

	void TriggerError(FString ErrorMessage);

	void TriggerLoadingPopup(bool bShowPopup, FString Message = "");

	void DestroySessionCaller();

	
};