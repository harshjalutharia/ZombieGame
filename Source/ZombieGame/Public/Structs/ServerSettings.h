
#pragma once

#include "ServerSettings.generated.h"

USTRUCT()
struct FCustomGameMode
{
	GENERATED_BODY()

	FString Name;
	FString Description;
	FString URL;
};

USTRUCT()
struct FGameMap
{
	GENERATED_BODY()

	FString Name;
	FString Description;
	FString URL;

	UPROPERTY()
	UTexture2D* Icon;
};

USTRUCT()
struct FTimeLimit
{
	GENERATED_BODY()

	FString TimeLimitString;
	int32 TimeLimitSeconds;
};

USTRUCT()
struct FScoreLimit
{
	GENERATED_BODY()

	FString ScoreLimitString;
	int32 ScoreLimitSeconds;
};