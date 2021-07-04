// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZombieGame/ZEnums.h"
#include "ChatText.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UChatText : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* ChatMessage;

public:

	void SetMessage(const FString& PlayerName, const FString& Message, const EChatLogType ChatLogType);
	
};
