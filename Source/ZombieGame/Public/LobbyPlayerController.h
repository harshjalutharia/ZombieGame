// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Structs/LobbyServerInfo.h"
#include "Structs/LobbyPlayerInfo.h"
#include "LobbyPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ALobbyPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:

	UPROPERTY(ReplicatedUsing=OnRep_LobbyServerInfo)
	FLobbyServerInfo LobbyServerInfo;

	FLobbyPlayerInfo LobbyPlayerInfo;

	UPROPERTY()
	class ULobbyMenu* LobbyMenu;

	UPROPERTY()
	class ALobbyGameMode* LobbyGameMode;

public:

	void SetLobbyGameModeRef(class ALobbyGameMode* InLobbyGameMode);

	void Setup();

private:

	UFUNCTION(Client, Reliable)
	void Client_LoadLobbyMenu();

public:

	void UpdateLobbyInfo(FLobbyServerInfo InLobbyServerInfo);

private:

	UFUNCTION()
	void OnRep_LobbyServerInfo();

public:

	void UpdateStatus(bool bIsReady);

private:

	UFUNCTION(Server,Reliable,WithValidation)
	void Server_UpdateStatus(bool bIsReady);

public:

	void TryStartSession();
};
