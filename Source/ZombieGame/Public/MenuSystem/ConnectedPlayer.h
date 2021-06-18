// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "ConnectedPlayer.generated.h"

class UTextBlock;
class UImage;

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
	UImage* StatusImage;

protected:

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* ReadyImage;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* NotReadyImage;

public:

	void SetPlayerNameAndAvatar(const FString& InPlayerName, UTexture2D* InAvatar = nullptr);

	void SetPlayerStatus(bool bIsReady);
};
