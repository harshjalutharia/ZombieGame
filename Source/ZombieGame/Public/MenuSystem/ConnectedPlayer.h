// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "ConnectedPlayer.generated.h"

class UTextBlock;
class UImage;
class UWidgetSwitcher;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UConnectedPlayer : public UMenuWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerName;

	UPROPERTY(meta = (BindWidget))
	UImage* AvatarImage;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* ReadyStatusSwitcher;

	UPROPERTY(meta = (BindWidget))
	UImage* ReadyImage;

	UPROPERTY(meta = (BindWidget))
	UImage* NotReadyImage;

public:

	void SetPlayerNameAndAvatar(const FString& InPlayerName, UTexture2D* InAvatar = nullptr);

	void SetPlayerStatus(bool bIsReady);
};
