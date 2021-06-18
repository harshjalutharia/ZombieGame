// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/SelectLoadoutMenu.h"
#include "Components/WidgetSwitcher.h"


bool USelectLoadoutMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(WindowSwitcher != nullptr && PrimaryLoadoutWindow != nullptr)
		WindowSwitcher->SetActiveWidget(PrimaryLoadoutWindow);
	
	return true;
}


void USelectLoadoutMenu::ShowPrimaryLoadoutWindow()
{
	if(WindowSwitcher != nullptr && PrimaryLoadoutWindow != nullptr)
		WindowSwitcher->SetActiveWidget(PrimaryLoadoutWindow);
}


void USelectLoadoutMenu::ShowSecondaryLoadoutWindow()
{
	if(WindowSwitcher != nullptr && SecondaryLoadoutWindow != nullptr)
		WindowSwitcher->SetActiveWidget(SecondaryLoadoutWindow);
}
