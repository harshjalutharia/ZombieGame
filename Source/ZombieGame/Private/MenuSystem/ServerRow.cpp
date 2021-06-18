// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/ServerRow.h"
#include "MenuSystem/FindGamesMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UServerRow::Setup(UFindGamesMenu* InParent, uint32 InIndex, FText InServerName, FText InGameModeName, FText InMapName, uint8 InCurrentPlayers, uint8 InMaxPlayers)
{
	ParentMenu = InParent;
	Index = InIndex;

	if(!ensure(SelectButton != nullptr)) return;
	SelectButton->OnClicked.AddDynamic(this, &UServerRow::OnSelected);

	if(ServerName != nullptr)
		ServerName->SetText(InServerName);

	if(GameModeName != nullptr)
		GameModeName->SetText(InGameModeName);

	if(MapName != nullptr)
		MapName->SetText(InMapName);

	if(PlayerCount != nullptr)
		PlayerCount->SetText(FText::FromString(FString::FromInt(InCurrentPlayers) + "/" + FString::FromInt(InMaxPlayers)));
}


void UServerRow::SetSelected(bool IsSelected)
{
	bSelected = IsSelected;
}


void UServerRow::OnSelected()
{
	if(ParentMenu != nullptr)
	{
		ParentMenu->SetSelectedIndex(Index);
	}
}
