// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "SelectLoadoutMenu.generated.h"

class UWidgetSwitcher;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API USelectLoadoutMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

private:
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WindowSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* PrimaryLoadoutWindow;

	UPROPERTY(meta = (BindWidget))
	UWidget* SecondaryLoadoutWindow;

public:

	void ShowPrimaryLoadoutWindow();

	void ShowSecondaryLoadoutWindow();
	
};
