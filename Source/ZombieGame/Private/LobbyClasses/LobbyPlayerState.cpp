// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyClasses/LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"


void ALobbyPlayerState::OnRep_LobbyPlayerInfo()
{
	
}


void ALobbyPlayerState::UpdateLobbyServerInfo()
{
}


void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerState, LobbyPlayerInfo);
}