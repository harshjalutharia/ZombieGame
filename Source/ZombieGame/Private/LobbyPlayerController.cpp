// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "MenuSystem/LobbyMenu.h"
#include "LobbyGameMode.h"
#include "Net/UnrealNetwork.h"


void ALobbyPlayerController::Setup(int32 PlayerID, class ALobbyGameMode* InLobbyGameMode)
{
	if(!GetWorld()->IsServer()) return;

	LobbyGameMode = InLobbyGameMode;
	Client_LoadLobbyMenu();
	Client_SendPlayerInfoToServer(PlayerID);
}


void ALobbyPlayerController::Client_LoadLobbyMenu_Implementation()
{
	if(GetGameInstance() != nullptr)
	{
		if(GetGameInstance()->GetClass()->ImplementsInterface(UZINT_GameInstance::StaticClass()))
		{
			LobbyMenu = IZINT_GameInstance::Execute_LoadLobbyMenu(GetGameInstance());
			LobbyMenu->SetPlayerControllerReference(this);
		}
	}
}


void ALobbyPlayerController::Client_SendPlayerInfoToServer_Implementation(int32 PlayerID)
{
	if(IsLocalController() && GetGameInstance() != nullptr)
	{
		if(GetGameInstance()->GetClass()->ImplementsInterface(UZINT_GameInstance::StaticClass()))
		{
			IZINT_GameInstance::Execute_GetPlayerInfo(GetGameInstance(),SelfPlayerInfo);
			SelfPlayerInfo.bIsReady = GetWorld()->IsServer() ? true : false;
			SelfPlayerInfo.PlayerID = PlayerID;
			Server_SetPlayerInfo(SelfPlayerInfo);
		}
	}
}


void ALobbyPlayerController::UpdateLobbyInfo(FLobbyServerInfo InLobbyServerInfo)
{
	if(LobbyServerInfo.ServerName != InLobbyServerInfo.ServerName)
		LobbyServerInfo.ServerName = InLobbyServerInfo.ServerName;
	if(LobbyServerInfo.MapName != InLobbyServerInfo.MapName)
		LobbyServerInfo.MapName = InLobbyServerInfo.MapName;
	if(LobbyServerInfo.GameModeName != InLobbyServerInfo.GameModeName)
		LobbyServerInfo.GameModeName = InLobbyServerInfo.GameModeName;
	if(LobbyServerInfo.TimeLimit != InLobbyServerInfo.TimeLimit)
		LobbyServerInfo.TimeLimit = InLobbyServerInfo.TimeLimit;
	if(LobbyServerInfo.ScoreLimit != InLobbyServerInfo.ScoreLimit)
		LobbyServerInfo.ScoreLimit = InLobbyServerInfo.ScoreLimit;

	if(IsLocalPlayerController() && LobbyMenu != nullptr)
		LobbyMenu->UpdateLobbyInfo(LobbyServerInfo);
}


void ALobbyPlayerController::UpdateAllPlayersInfo(const TArray<FLobbyPlayerInfo>& InAllPlayersInfo)
{
	AllPlayersInfo = InAllPlayersInfo;

	if(IsLocalController() && LobbyMenu != nullptr)
		LobbyMenu->UpdateAllPlayersInfo(AllPlayersInfo);
}


void ALobbyPlayerController::UpdatePlayerInfoReadyStatus(int32 PlayerID, bool bIsReady)
{
	for(auto& PlayerInfo : AllPlayersInfo)
	{
		if(PlayerInfo.PlayerID == PlayerID)
		{
			PlayerInfo.bIsReady = bIsReady;
			break;
		}
	}

	if(LobbyMenu != nullptr)
		LobbyMenu->UpdateAllPlayersInfo(AllPlayersInfo);
}


void ALobbyPlayerController::OnRep_LobbyServerInfo()
{
	if(LobbyMenu != nullptr)
		LobbyMenu->UpdateLobbyInfo(LobbyServerInfo);
}


void ALobbyPlayerController::OnRep_AllPlayersInfo()
{
	if(LobbyMenu != nullptr)
		LobbyMenu->UpdateAllPlayersInfo(AllPlayersInfo);
}


void ALobbyPlayerController::UpdateStatus(bool bIsReady)
{
	if(!GetWorld()->IsServer())
	{
		Server_UpdateStatus(bIsReady);
	}
}


void ALobbyPlayerController::Server_UpdateStatus_Implementation(bool bIsReady)
{
	if(LobbyGameMode != nullptr)
	{
		LobbyGameMode->UpdatePlayerStatus(SelfPlayerInfo.PlayerID,bIsReady);
	}
}


bool ALobbyPlayerController::Server_UpdateStatus_Validate(bool bIsReady)
{
	return true;
}


void ALobbyPlayerController::Server_SetPlayerInfo_Implementation(FLobbyPlayerInfo PlayerInfo)
{
	if(LobbyGameMode != nullptr)
	{
		SelfPlayerInfo = PlayerInfo;
		LobbyGameMode->SetPlayerInfo(this, PlayerInfo);
	}
}


bool ALobbyPlayerController::Server_SetPlayerInfo_Validate(FLobbyPlayerInfo PlayerInfo)
{
	return true;
}


void ALobbyPlayerController::Server_SendChatMessage_Implementation(const FString& Message)
{
	if(LobbyGameMode != nullptr)
	{
		LobbyGameMode->SendChatMessage(SelfPlayerInfo.PlayerID, Message);
	}
}


bool ALobbyPlayerController::Server_SendChatMessage_Validate(const FString& Message)
{
	return true;
}


void ALobbyPlayerController::Client_DisplayBroadcastedMessage_Implementation(const FString& PlayerName, const FString& Message, const EChatLogType ChatLogType)
{
	if(IsLocalController() && LobbyMenu!=nullptr)
	{
		LobbyMenu->AddMessageToChatWindow(PlayerName,Message, ChatLogType);
	}
}


void ALobbyPlayerController::RemovePlayerFromAllPlayersInfo(const int32 LeavingPlayerID)
{
	for(int32 itr = 0; itr < AllPlayersInfo.Num(); itr++)
	{
		if(AllPlayersInfo[itr].PlayerID == LeavingPlayerID)
		{
			AllPlayersInfo.RemoveAt(itr);
			break;
		}
	}

	if(IsLocalController() && LobbyMenu != nullptr)
		LobbyMenu->UpdateAllPlayersInfo(AllPlayersInfo);
}


void ALobbyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerController, LobbyServerInfo);
	DOREPLIFETIME(ALobbyPlayerController, AllPlayersInfo);
}

