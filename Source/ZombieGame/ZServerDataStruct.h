
#pragma once

#include "ZServerDataStruct.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString ServerName;
	uint8 CurrentPlayers;
	uint8 MaxPlayers;
	FString HostUsername;
};