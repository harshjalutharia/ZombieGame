// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	class IZINT_GameInstance* GameInstanceInterface;

	void DisplayMenu(bool bShowCursor);

	void Teardown();

public:
	
	void Setup(class IZINT_GameInstance* NewInterface, bool bVisible, bool bShowCursor = false);

	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	void ShowMenu(bool bShowCursor = false);
};
