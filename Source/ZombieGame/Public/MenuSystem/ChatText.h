// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatText.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UChatText : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ChatMessage;

public:

	void SetMessage(const FString& Message);
	
};
