// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/FindGamesMenu.h"
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

	virtual void Host(FString ServerName, uint8 GameModeIndex, uint8 MapIndex, uint8 MaxPlayers, bool bLanMatch) = 0;

	virtual void Join(uint32 ServerIndex) = 0;

	virtual void RefreshServerList() = 0;

	virtual void CancelServerSearch() = 0;

	virtual void GetAllGameModesAndMaps(TArray<FString> &InAllGameModes, TArray<FString> &InAllMaps) = 0;

	virtual uint8 GetMaxAllowedPlayers() = 0;

	virtual void SetFindGamesMenu(UFindGamesMenu* InFindGamesMenu) = 0;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
    void ShowPauseMenu();
};
