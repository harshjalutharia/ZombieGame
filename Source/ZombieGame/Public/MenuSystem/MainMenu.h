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
	Popup UI
	*/

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ErrorPopupUI;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ErrorPopupText;

	UPROPERTY(meta = (BindWidget))
	UButton* ErrorPopupButton;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* LoadingPopupUI;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LoadingPopupText;

	UFUNCTION()
	void OnPopupButtonClicked();

public:

	void ShowErrorMessage(FString ErrorMessage);

	void ShowLoadingMessage(FString LoadingMessage);

	void StopShowingLoadingMessage();
	
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
	void OnBackHostClicked();
    
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
	UButton* RefreshListButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButtonJoinMenu;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ServerList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UServerRow> ServerRowBPClass;

	UFUNCTION()
	void OnBackJoinClicked();

	UFUNCTION()
	void OnRefreshListClicked();

	UFUNCTION()
	void JoinServer();

	TOptional<uint32> SelectedIndex;

	void UpdateAllRows();
	
public:

	void SetServerList(TArray<FServerData> InServerList);

	void ClearServerList();

	void SetSelectedIndex(uint32 Index);
};
