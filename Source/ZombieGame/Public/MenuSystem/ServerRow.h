// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"


class UFindGamesMenu;
class UTextBlock;
class UButton;
class UImage;


/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	UFindGamesMenu* ParentMenu;

	uint32 Index;

	FString Password;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameModeName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MapName;

	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;

	UPROPERTY(meta = (BindWidget))
	UImage* PasswordImage;

protected:

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* LockedImage;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* UnlockedImage;

public:

	void Setup(UFindGamesMenu* InParent, uint32 InIndex, FText InServerName, FString InPassword, FText InGameModeName, FText InMapName, uint8 InCurrentPlayers, uint8 InMaxPlayers);

	void SetSelected(bool IsSelected);

	bool GetIsPasswordProtected() const { return !Password.Equals(""); }

	FString& GetPassword() { return Password; }

protected:

	UPROPERTY(BlueprintReadOnly)
	bool bSelected;
	
private:

	UFUNCTION()
	void OnSelected();

};
