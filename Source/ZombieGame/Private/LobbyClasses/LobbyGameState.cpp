// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyClasses/LobbyGameState.h"
#include "LobbyPlayerController.h"
#include "LobbyClasses/LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"


void ALobbyGameState::OnRep_LobbyServerInfo()
{
	for(auto Player : PlayerArray)
	{
		ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(Player);
		
	}
}


void ALobbyGameState::SetLobbyServerInfo(FLobbyServerInfo& InLobbyServerInfo)
{
	
	LobbyServerInfo = InLobbyServerInfo;

	if(ServerPlayerController != nullptr)
		ServerPlayerController->UpdateLobbyInfo(LobbyServerInfo);
}


void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGameState, LobbyServerInfo);
}