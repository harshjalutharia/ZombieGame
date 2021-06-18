// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "MenuSystem/LobbyMenu.h"
#include "Net/UnrealNetwork.h"


ALobbyPlayerController::ALobbyPlayerController(const FObjectInitializer& ObjectInitializer)
{
	
}


void ALobbyPlayerController::SetLobbyGameModeRef(ALobbyGameMode* InLobbyGameMode)
{
	if(!GetWorld()->IsServer()) return;
	
	LobbyGameMode = InLobbyGameMode;
}


void ALobbyPlayerController::Setup()
{
	Client_LoadLobbyMenu();
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


void ALobbyPlayerController::OnRep_LobbyServerInfo()
{
	if(LobbyMenu != nullptr)
		LobbyMenu->UpdateLobbyInfo(LobbyServerInfo);
}


void ALobbyPlayerController::UpdateStatus(bool bIsReady)
{
	if(!GetWorld()->IsServer())
	{
		Server_UpdateStatus(bIsReady);
	}
}


void ALobbyPlayerController::TryStartSession()
{
	
}


void ALobbyPlayerController::Server_UpdateStatus_Implementation(bool bIsReady)
{
	// TODO: TELL GAME MODE WE ARE READY TO PLAY
}


bool ALobbyPlayerController::Server_UpdateStatus_Validate(bool bIsReady)
{
	return true;
}


void ALobbyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerController, LobbyServerInfo);
}

