// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/GameplaySettings.h"
#include "Structs/LobbyPlayerInfo.h"
#include "Structs/LobbyServerInfo.h"
#include "UObject/Interface.h"
#include "ZINT_GameInstance.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UZINT_GameInstance : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIEGAME_API IZINT_GameInstance
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Host(FString& ServerName, FString& Password, uint8 GameModeIndex, uint8 MapIndex, uint8 MaxPlayers, bool bLanMatch) = 0;

	virtual void Join(uint32 ServerIndex) = 0;

	virtual void RefreshServerList() = 0;

	virtual void CancelServerSearch() = 0;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	class UPauseMenu* LoadPauseMenu();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	class ULobbyMenu* LoadLobbyMenu();

	virtual void GetAllGameModesAndMaps(TArray<FString> &InAllGameModes, TArray<FString> &InAllMaps) const = 0;

	virtual void GetAllTimeLimitsAndScoreLimits(TArray<FString> &InAllTimeLimits, TArray<FString> &InAllScoreLimits) const = 0;

	virtual uint8 GetMaxAllowedPlayers() const = 0;

	virtual void SetFindGamesMenu(class UFindGamesMenu* InFindGamesMenu) = 0;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void GetLobbyServerInfo(FLobbyServerInfo& OutLobbyServerInfo) const;

	virtual float GetMinMouseSensitivity() const = 0;

	virtual float GetMaxMouseSensitivity() const = 0;
	
	virtual FGameplaySettings& GetGameplaySettings() = 0;

	virtual void SaveGameplaySettings(FGameplaySettings& NewGameplaySettings) = 0;

	virtual void GetCurrentLobbyInfoIndexes(uint8& OutGameModeIndex, uint8& OutMapIndex, uint8& OutScoreLimitIndex, uint8& OutTimeLimitIndex) = 0;

	virtual void HostUpdateLobbyServerInfo(uint8 GameModeIndex, uint8 MapIndex, uint8 ScoreLimitIndex, uint8 TimeLimitIndex) = 0;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void GetPlayerInfo(FLobbyPlayerInfo& OutPlayerInfo);

	virtual void StartGame() = 0;
};
