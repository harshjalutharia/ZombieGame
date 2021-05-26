// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HorizontalList.generated.h"


class UButton;
class UTextBlock;


/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UHorizontalList : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

private:

	UPROPERTY(meta = (BindWidget))
	UButton* LeftButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RightButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayString;

	UFUNCTION()
	void OnLeftButtonClicked();

	UFUNCTION()
	void OnRightButtonClicked();

	uint8 CurrentIndex;

public:

	TArray<FString> List;

	void Setup();

	uint8 GetCurrentIndex() const { return CurrentIndex; }
	
};
