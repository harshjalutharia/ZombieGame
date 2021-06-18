// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Structs/LobbyServerInfo.h"
#include "LobbyGameState.generated.h"

class ALobbyPlayerController;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing=OnRep_LobbyServerInfo)
	FLobbyServerInfo LobbyServerInfo;

	UPROPERTY()
	ALobbyPlayerController* ServerPlayerController;

	UFUNCTION()
	void OnRep_LobbyServerInfo();

public:

	void SetServerPlayerController(ALobbyPlayerController* InServerPlayerController) { ServerPlayerController = InServerPlayerController; }

	void SetLobbyServerInfo(FLobbyServerInfo& InLobbyServerInfo);
	
};
