// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/LobbyServerInfo.h"
#include "Structs/LobbyPlayerInfo.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	ALobbyGameMode(const FObjectInitializer & ObjectInitializer);

protected:

	virtual void BeginPlay() override;
	
public:

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	void UpdateLobbyServerInfoOnClients(FLobbyServerInfo& LobbyServerInfo);

private:

	//UPROPERTY()
	//class ALobbyGameState* LobbyGameState;

	UPROPERTY()
	TArray<class ALobbyPlayerController*> ConnectedPlayers;
	
	TMap<class ALobbyPlayerController*, FLobbyPlayerInfo> ConnectedPlayersInfo;

};
