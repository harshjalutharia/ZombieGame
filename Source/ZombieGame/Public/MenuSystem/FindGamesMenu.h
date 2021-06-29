// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "Structs/LobbyServerInfo.h"
#include "FindGamesMenu.generated.h"

class UButton;
class UScrollBox;
class UTextBlock;
class UEditableText;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UFindGamesMenu : public UMenuWidget
{
	GENERATED_BODY()

public:

	UFindGamesMenu(const FObjectInitializer& ObjectInitializer);
	
protected:

	virtual bool Initialize() override;

public:

	virtual void Setup(IZINT_GameInstance* NewInterface) override;

private:

	UPROPERTY(meta = (BindWidget))
	UButton* RefreshListButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinGameButton;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ServerList;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* LoadingPopupUI;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* PasswordPopupUI;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WrongPasswordText;

	UPROPERTY(meta = (BindWidget))
	UEditableText* EditPassword;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButtonPasswordPopup;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButtonPasswordPopup;

	TSubclassOf<class UServerRow> ServerRowClass;

	TOptional<int32> SelectedIndex;

	TArray<UServerRow*> AllServerRows;

	FString SelectedServerPassword;

	UFUNCTION()
	void OnRefreshListButtonClicked();

	UFUNCTION()
	void OnJoinGameButtonClicked();

	UFUNCTION()
	void OnCancelButtonClicked();

	UFUNCTION()
	void OnPopupJoinButtonClicked();

	void UpdateAllRows();

public:

	void RefreshServerList();

	void SetServerList(TArray<FLobbyServerInfo> InServerList);

	void ClearServerList();

	void SetSelectedIndex(uint32 Index);

	void ToggleLoadingPopupUI(bool bVisible);
};
