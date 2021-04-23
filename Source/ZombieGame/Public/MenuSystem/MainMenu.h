// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/EditableText.h"
#include "MenuSystem/MenuWidget.h"
#include "MainMenu.generated.h"

class UButton;
class UWidgetSwitcher;
class UEditableText;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

private:

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	/*
	Main Menu
	*/

	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;
	
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	/*
	Host Menu
	*/
	
	UPROPERTY(meta = (BindWidget))
	UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButtonHostMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* HostButtonHostMenu;

	UPROPERTY(meta = (BindWidget))
	UEditableText* EditServerName;

	/*
	Join Menu
	*/

	UPROPERTY(meta = (BindWidget))
	UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButtonJoinMenu;

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
    void OnJoinClicked();

	UFUNCTION()
    void ExitGame();
    
    UFUNCTION()
    void OnBackClicked();
    
    UFUNCTION()
    void HostServer();
};
