
#pragma once

#include "LobbyPlayerInfo.generated.h"

USTRUCT(BlueprintType)
struct FLobbyPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* PlayerAvatarImage;

	UPROPERTY()
	bool bIsReady = false;

	UPROPERTY()
	int32 PlayerID;

	UPROPERTY(BlueprintReadWrite)
	FString UniqueNetPlayerIDString;

	UPROPERTY()
	bool bIsHost = false;

	FLobbyPlayerInfo()
	{
		PlayerName = "";
		PlayerAvatarImage = nullptr;
		bIsReady = false;
		PlayerID = -1;
		UniqueNetPlayerIDString = "";
		bIsHost = false;
	}

	FLobbyPlayerInfo(FString InName, UTexture2D* InAvatarImage, bool InIsReady, int32 InPlayerID, FString InUniqueNetPlayerIDString, bool InIsHost)
	{
		PlayerName = InName;
		PlayerAvatarImage = InAvatarImage;
		bIsReady = InIsReady;
		PlayerID = InPlayerID;
		UniqueNetPlayerIDString = InUniqueNetPlayerIDString;
		bIsHost = InIsHost;
	}

	FLobbyPlayerInfo(const FLobbyPlayerInfo& PlayerInfo)
	{
		PlayerName = PlayerInfo.PlayerName;
		PlayerAvatarImage = PlayerInfo.PlayerAvatarImage;
		bIsReady = PlayerInfo.bIsReady;
		PlayerID = PlayerInfo.PlayerID;
		UniqueNetPlayerIDString = PlayerInfo.UniqueNetPlayerIDString;
		bIsHost = PlayerInfo.bIsHost;
	}
};