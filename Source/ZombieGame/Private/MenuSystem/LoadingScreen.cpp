// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/LoadingScreen.h"


void ULoadingScreen::NativeConstruct()
{
	if(FadeOutAnimation != nullptr)
	{
		FadeOutAnimationFinishDelegate.BindDynamic(this, &ULoadingScreen::OnFadeOutAnimationCompleted);
		BindToAnimationFinished(FadeOutAnimation,FadeOutAnimationFinishDelegate);
	}
}


void ULoadingScreen::PlayFadeInAnimation()
{
	if(FadeInAnimation != nullptr)
	{
		PlayAnimation(FadeInAnimation);
	}
}


void ULoadingScreen::PlayFadeOutAnimation()
{
	if(FadeOutAnimation != nullptr)
	{
		PlayAnimation(FadeOutAnimation);
	}
}


void ULoadingScreen::OnFadeOutAnimationCompleted()
{
	GetGameInstance()->GetGameViewportClient()->RemoveViewportWidgetContent(this->TakeWidget());
}


