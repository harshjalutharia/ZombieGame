
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

	FLobbyPlayerInfo()
	{
		PlayerName = "";
		PlayerAvatarImage = nullptr;
		bIsReady = false;
		PlayerID = -1;
	}

	FLobbyPlayerInfo(FString InName, UTexture2D* InAvatarImage, bool InIsReady, int32 InPlayerID)
	{
		PlayerName = InName;
		PlayerAvatarImage = InAvatarImage;
		bIsReady = InIsReady;
		PlayerID = InPlayerID;
	}

	FLobbyPlayerInfo(const FLobbyPlayerInfo& PlayerInfo)
	{
		PlayerName = PlayerInfo.PlayerName;
		PlayerAvatarImage = PlayerInfo.PlayerAvatarImage;
		bIsReady = PlayerInfo.bIsReady;
		PlayerID = PlayerInfo.PlayerID;
	}
};
