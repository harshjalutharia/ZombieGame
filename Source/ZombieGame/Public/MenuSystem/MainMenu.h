// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "MainMenu.generated.h"

class UButton;
class UWidgetSwitcher;
class UEditableText;
class UScrollBox;
class UTextBlock;

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString ServerName;
	uint8 CurrentPlayers;
	uint8 MaxPlayers;
	FString HostUsername;
};

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

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;
	
	UFUNCTION()
	void ExitGame();

	/*
	Login Menu
	*/

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* PopupUI;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PopupText;

	UPROPERTY(meta = (BindWidget))
	UButton* PopupButton;

	UPROPERTY(meta = (BindWidget))
	UWidget* ErrorIcon;

	UPROPERTY(meta = (BindWidget))
	UWidget* LoadingIcon;

	UFUNCTION()
	void OnPopupButtonClicked();
	
	/*
	Main Menu
	*/

private:

	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;
	
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();
	
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
    
	UFUNCTION()
	void OnBackClicked();
    
	UFUNCTION()
	void HostServer();
	
	/*
	Join Menu
	*/

	UPROPERTY(meta = (BindWidget))
	UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButtonJoinMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButtonJoinMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* LoadingIconJoinMenu;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ServerList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UServerRow> ServerRowBPClass;

	UFUNCTION()
	void JoinServer();

	TOptional<uint32> SelectedIndex;

	void UpdateAllRows();
	
public:

	void SetServerList(TArray<FServerData> InServerList);

	void ClearServerList();

	void SetSelectedIndex(uint32 Index);
};
