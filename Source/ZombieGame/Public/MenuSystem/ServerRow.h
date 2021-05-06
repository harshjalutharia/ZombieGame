// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"


class UMainMenu;
class UTextBlock;
class UButton;


/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	UMainMenu* ParentMenu;

	uint32 Index;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HostUsername;

	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;

public:

	void Setup(UMainMenu* InParent, uint32 InIndex, FText InServerName, FText InHostUsername, uint8 InCurrentPlayers, uint8 InMaxPlayers);

	void SetSelected(bool IsSelected);

protected:

	UPROPERTY(BlueprintReadOnly)
	bool bSelected;
	
private:

	UFUNCTION()
	void OnSelected();

};
