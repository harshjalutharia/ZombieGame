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

public:
	
	void Setup(bool bShowCursor, class IZINT_GameInstance* NewInterface = nullptr);

	void Teardown();

	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
};
