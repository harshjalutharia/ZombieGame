// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/MenuWidget.h"


void UMenuWidget::Setup(bool bShowCursor)
{
	AddToViewport();
	bIsFocusable = true;
	
	auto PC = GetOwningPlayer();
	if(!ensure(PC!=nullptr)) return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(TakeWidget());

	if(bShowCursor)
	{
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->bShowMouseCursor = true;
	}
	else
	{
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		PC->bShowMouseCursor = false;
	}

	PC->SetInputMode(InputModeData);
}


void UMenuWidget::Teardown()
{
	RemoveFromViewport();
	bIsFocusable = false;

	auto PC = GetOwningPlayer();
	if(!ensure(PC!=nullptr)) return;

	PC->bShowMouseCursor = false;

	const FInputModeGameOnly InputModeData;
	PC->SetInputMode(InputModeData);
}


void UMenuWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Teardown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

