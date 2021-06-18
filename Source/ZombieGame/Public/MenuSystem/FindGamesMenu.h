// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "Structs/LobbyServerInfo.h"
#include "FindGamesMenu.generated.h"

class UButton;
class UScrollBox;

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

	TSubclassOf<class UServerRow> ServerRowClass;

	UFUNCTION()
	void OnRefreshListButtonClicked();

	UFUNCTION()
	void OnJoinGameButtonClicked();

	TOptional<uint32> SelectedIndex;

	void UpdateAllRows();

public:

	void RefreshServerList();

	void SetServerList(TArray<FLobbyServerInfo> InServerList);

	void ClearServerList();

	void SetSelectedIndex(uint32 Index);

	void ToggleLoadingPopupUI(bool bVisible);
};
