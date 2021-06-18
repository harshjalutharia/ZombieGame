// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Structs/GameplaySettings.h"
#include "OptionsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UOptionsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FGameplaySettings GameplaySettings;
};
