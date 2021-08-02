// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "LoadingScreen.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API ULoadingScreen : public UMenuWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

	FWidgetAnimationDynamicEvent FadeOutAnimationFinishDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetAnimation* FadeInAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetAnimation* FadeOutAnimation;

public:

	void PlayFadeInAnimation();

	void PlayFadeOutAnimation();

	UFUNCTION()
	void OnFadeOutAnimationCompleted();
	
};
