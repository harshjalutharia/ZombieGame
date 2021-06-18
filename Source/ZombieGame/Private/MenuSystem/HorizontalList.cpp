// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/HorizontalList.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


bool UHorizontalList::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(LeftButton!=nullptr)) return false;
	LeftButton->OnClicked.AddDynamic(this, &UHorizontalList::OnLeftButtonClicked);

	if(!ensure(RightButton!=nullptr)) return false;
	RightButton->OnClicked.AddDynamic(this, &UHorizontalList::OnRightButtonClicked);

	return true;
}


void UHorizontalList::OnLeftButtonClicked()
{
	if(List.Num() > 1 && DisplayString != nullptr)
	{
		CurrentIndex = (CurrentIndex-1 < 0)? List.Num()-1 : CurrentIndex-1;
		DisplayString->SetText(FText::FromString(List[CurrentIndex]));
	}
}


void UHorizontalList::OnRightButtonClicked()
{
	if(List.Num() > 1 && DisplayString != nullptr)
	{
		CurrentIndex = (CurrentIndex+1 >= List.Num())? 0 : CurrentIndex+1;
		DisplayString->SetText(FText::FromString(List[CurrentIndex]));
	}
}


void UHorizontalList::Setup()
{
	CurrentIndex = 0;
	if(List.Num() > 0 && DisplayString != nullptr)
	{
		DisplayString->SetText(FText::FromString(List[CurrentIndex]));
	}
}


void UHorizontalList::SetCurrentIndex(uint8 NewIndex)
{
	CurrentIndex = NewIndex;
	DisplayString->SetText(FText::FromString(List[CurrentIndex]));
}
