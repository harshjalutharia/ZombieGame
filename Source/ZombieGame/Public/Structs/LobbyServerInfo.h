
#pragma once

#include "LobbyServerInfo.generated.h"

USTRUCT(BlueprintType)
struct FLobbyServerInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString ServerName;

	UPROPERTY()
	FString Password;
	
	UPROPERTY()
	FString MapName;
	
	UPROPERTY()
	FString GameModeName;
	
	UPROPERTY()
	FString TimeLimit;
	
	UPROPERTY()
	FString ScoreLimit;
	
	uint8 CurrentPlayers;
	uint8 MaxPlayers;
};
