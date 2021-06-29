// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "Structs/LobbyPlayerInfo.h"
#include "KickPlayerButton.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UKickPlayerButton : public UMenuWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UButton* PlayerButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerName;

	UPROPERTY(meta = (BindWidget))
	class UImage* AvatarImage;

	FLobbyPlayerInfo PlayerInfo;

protected:

	virtual bool Initialize() override;

private:

	UFUNCTION()
	void OnPlayerButtonClicked();

public:

	void SetPlayerInfo(const FLobbyPlayerInfo& InPlayerInfo);
	
};
