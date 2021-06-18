// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyPlayerController.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "LobbyClasses/LobbyGameState.h"


ALobbyGameMode::ALobbyGameMode(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APlayerController> LobbyPlayerControllerBPClass(TEXT("/Game/Blueprints/BP_LobbyPlayerController"));
	if (!LobbyPlayerControllerBPClass.Class) return;
		PlayerControllerClass = LobbyPlayerControllerBPClass.Class;

	bUseSeamlessTravel = true;
}


void ALobbyGameMode::BeginPlay()
{
	FLobbyServerInfo LobbyServerInfo;
	if(GetGameInstance()->GetClass()->ImplementsInterface(UZINT_GameInstance::StaticClass()))
	{
		IZINT_GameInstance::Execute_GetLobbyServerInfo(GetGameInstance(),LobbyServerInfo);
	}
	
	//LobbyGameState = GetGameState<ALobbyGameState>();
	//LobbyGameState->SetLobbyServerInfo(LobbyServerInfo);
}


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(NewPlayer);
	if(PC)
	{
		//if(PC->IsLocalPlayerController() && LobbyGameState != nullptr)
		//	LobbyGameState->SetServerPlayerController(PC);
		
		ConnectedPlayers.Add(PC);
		
		FLobbyServerInfo LobbyServerInfo;
	
		if(GetGameInstance()->GetClass()->ImplementsInterface(UZINT_GameInstance::StaticClass()))
		{
			IZINT_GameInstance::Execute_GetLobbyServerInfo(GetGameInstance(),LobbyServerInfo);
		}

		PC->Setup();
		PC->UpdateLobbyInfo(LobbyServerInfo);

		// Set game mode ref on host player controller
		if(PC->IsLocalPlayerController())
		{
			PC->SetLobbyGameModeRef(this);
		}
	}
}


void ALobbyGameMode::Logout(AController* Exiting)
{
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(Exiting);
	if(PC)
	{
		ConnectedPlayers.Remove(PC);
	}
}


void ALobbyGameMode::UpdateLobbyServerInfoOnClients(FLobbyServerInfo& LobbyServerInfo)
{
	for(auto PC : ConnectedPlayers)
	{
		PC->UpdateLobbyInfo(LobbyServerInfo);
	}
}


