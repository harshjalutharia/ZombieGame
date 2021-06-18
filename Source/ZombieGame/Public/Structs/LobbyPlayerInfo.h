
#pragma once

#include "LobbyPlayerInfo.generated.h"

USTRUCT(BlueprintType)
struct FLobbyPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* PlayerAvatarImage;

	UPROPERTY()
	bool bIsReady;
};
