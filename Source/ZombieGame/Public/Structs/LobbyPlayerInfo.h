
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
	uint8 PlayerID;

	UPROPERTY(BlueprintReadWrite)
	FString UniqueNetPlayerIDString;

	FLobbyPlayerInfo()
	{
		PlayerName = "";
		PlayerAvatarImage = nullptr;
		bIsReady = false;
		PlayerID = -1;
		UniqueNetPlayerIDString = "";
	}

	FLobbyPlayerInfo(FString InName, UTexture2D* InAvatarImage, bool InIsReady, int32 InPlayerID, FString InUniqueNetPlayerIDString)
	{
		PlayerName = InName;
		PlayerAvatarImage = InAvatarImage;
		bIsReady = InIsReady;
		PlayerID = InPlayerID;
		UniqueNetPlayerIDString = InUniqueNetPlayerIDString;
	}

	FLobbyPlayerInfo(const FLobbyPlayerInfo& PlayerInfo)
	{
		PlayerName = PlayerInfo.PlayerName;
		PlayerAvatarImage = PlayerInfo.PlayerAvatarImage;
		bIsReady = PlayerInfo.bIsReady;
		PlayerID = PlayerInfo.PlayerID;
		UniqueNetPlayerIDString = PlayerInfo.UniqueNetPlayerIDString;
	}
};