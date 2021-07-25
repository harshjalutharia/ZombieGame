// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Structs/LobbyServerInfo.h"
#include "Structs/LobbyPlayerInfo.h"
#include "ZombieGame/ZEnums.h"
#include "LobbyPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

private:

	UPROPERTY(ReplicatedUsing=OnRep_LobbyServerInfo)
	FLobbyServerInfo LobbyServerInfo;

	FLobbyPlayerInfo SelfPlayerInfo;

	UPROPERTY(ReplicatedUsing=OnRep_AllPlayersInfo)
	TArray<FLobbyPlayerInfo> AllPlayersInfo;

	UPROPERTY()
	class ULobbyMenu* LobbyMenu;

	UPROPERTY()
	class ALobbyGameMode* LobbyGameMode;

public:

	void Setup(int32 PlayerID, class ALobbyGameMode* InLobbyGameMode);

private:

	UFUNCTION(Client, Reliable)
	void Client_LoadLobbyMenu();

	UFUNCTION(Client, Reliable)
	void Client_SendPlayerInfoToServer(int32 PlayerID);

public:

	void UpdateLobbyInfo(FLobbyServerInfo InLobbyServerInfo);

	void UpdateAllPlayersInfo(const TArray<FLobbyPlayerInfo>& InAllPlayersInfo);

	void UpdatePlayerInfoReadyStatus(int32 PlayerID, bool bIsReady);

private:

	void LoadAllPlayerImages();

	UFUNCTION()
	void OnRep_LobbyServerInfo();

	UFUNCTION()
	void OnRep_AllPlayersInfo();

public:

	void UpdateStatus(bool bIsReady);

private:

	UFUNCTION(Server,Reliable,WithValidation)
	void Server_UpdateStatus(bool bIsReady);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPlayerInfo(FLobbyPlayerInfo PlayerInfo);

public:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendChatMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void Client_DisplayBroadcastedMessage(const FString& PlayerName, const FString& Message, const EChatLogType ChatLogType);

	int32 GetSelfPlayerID() const { return SelfPlayerInfo.PlayerID; }

	void RemovePlayerFromAllPlayersInfo(const int32 LeavingPlayerID);
	
};
