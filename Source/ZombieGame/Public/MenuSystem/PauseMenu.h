// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "PauseMenu.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UPauseMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

private:

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;

	UFUNCTION()
    void OnBackClicked();

	UFUNCTION()
    void OnExitClicked();
};
