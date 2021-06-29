// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyPlayerController.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/ZINT_GameInstance.h"


ALobbyGameMode::ALobbyGameMode(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APlayerController> LobbyPlayerControllerBPClass(TEXT("/Game/Blueprints/BP_LobbyPlayerController"));
	if (!LobbyPlayerControllerBPClass.Class) return;
		PlayerControllerClass = LobbyPlayerControllerBPClass.Class;

	bUseSeamlessTravel = true;
	PlayerIDIterator = 0;
}


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(NewPlayer);
	if(PC)
	{
		ConnectedPlayers.Add(PC);
		
		FLobbyPlayerInfo NewInfo;
		NewInfo.PlayerID = PlayerIDIterator;
		PlayerIDIterator++;
		ConnectedPlayersInfo.Add(PC, NewInfo);
		
		FLobbyServerInfo LobbyServerInfo;
	
		if(GetGameInstance()->GetClass()->ImplementsInterface(UZINT_GameInstance::StaticClass()))
		{
			IZINT_GameInstance::Execute_GetLobbyServerInfo(GetGameInstance(),LobbyServerInfo);
		}

		PC->Setup(NewInfo.PlayerID, this);
		PC->UpdateLobbyInfo(LobbyServerInfo);
	}

	Super::PostLogin(NewPlayer);
}


void ALobbyGameMode::Logout(AController* Exiting)
{
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(Exiting);
	if(PC)
	{
		const int32 ExitingPlayerID = ConnectedPlayersInfo[PC].PlayerID;
		ConnectedPlayers.Remove(PC);
		ConnectedPlayersInfo.Remove(PC);

		NotifyClientsOfPlayerLeave(ExitingPlayerID);
	}

	Super::Logout(Exiting);
}


void ALobbyGameMode::UpdateLobbyServerInfoOnClients(FLobbyServerInfo& LobbyServerInfo)
{
	for(auto PC : ConnectedPlayers)
	{
		PC->UpdateLobbyInfo(LobbyServerInfo);
	}
}


void ALobbyGameMode::SetPlayerInfo(ALobbyPlayerController* InPlayerController, FLobbyPlayerInfo& PlayerInfo)
{
	bool bInfoChanged = false;
	for(auto& PC : ConnectedPlayersInfo)
	{
		if(PC.Key == InPlayerController && PC.Value.PlayerID == PlayerInfo.PlayerID)
		{
			bInfoChanged = true;
			PC.Value = PlayerInfo;
			break;
		}
	}

	if(bInfoChanged)
	{
		TArray<FLobbyPlayerInfo> OutAllPlayersInfo;
		ConnectedPlayersInfo.GenerateValueArray(OutAllPlayersInfo);
		
		for(auto PC : ConnectedPlayers)
		{
			PC->UpdateAllPlayersInfo(OutAllPlayersInfo);
		}
	}
}


void ALobbyGameMode::UpdatePlayerStatus(int32 PlayerID, bool bIsReady)
{
	bool bStatusChanged = false;
	for(auto& Player : ConnectedPlayersInfo)
	{
		if(Player.Value.PlayerID == PlayerID)
		{
			if(Player.Value.bIsReady != bIsReady)
			{
				Player.Value.bIsReady = bIsReady;
				bStatusChanged = true;
			}
			break;
		}
	}

	if(bStatusChanged)
	{
		for(auto PC : ConnectedPlayers)
		{
			PC->UpdatePlayerInfoReadyStatus(PlayerID, bIsReady);
		}
	}
}


void ALobbyGameMode::KickPlayer(int32 KickPlayerID)
{
	for(auto& Player : ConnectedPlayersInfo)
	{
		if(Player.Value.PlayerID == KickPlayerID)
		{
			GameSession->KickPlayer(Player.Key,FText::FromString("Kicked By Host"));
			break;
		}
	}
}


void ALobbyGameMode::NotifyClientsOfPlayerLeave(int32 LeavingPlayerID)
{
	for(auto PC : ConnectedPlayers)
	{
		PC->RemovePlayerFromAllPlayersInfo(LeavingPlayerID);
	}
}

