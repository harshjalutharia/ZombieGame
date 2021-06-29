// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "HostMenu.generated.h"

class UHorizontalList;
class UEditableText;
class UCheckBox;
class UButton;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UHostMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

private:

	UPROPERTY(meta = (BindWidget))
	UEditableText* EditServerName;
	
	UPROPERTY(meta = (BindWidget))
	UEditableText* EditPassword;

	UPROPERTY(meta = (BindWidget))
	UHorizontalList* HLS_GameMode;

	UPROPERTY(meta = (BindWidget))
	UHorizontalList* HLS_Map;

	UPROPERTY(meta = (BindWidget))
	UHorizontalList* HLS_MaxPlayers;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* LANCheckBox;

	UPROPERTY(meta = (BindWidget))
	UButton* HostButtonHostMenu;

	UFUNCTION()
	void OnHostButtonHostMenuClicked();

	bool bInitialised;

public:

	virtual void Setup(class IZINT_GameInstance* NewInterface) override;
	
};
