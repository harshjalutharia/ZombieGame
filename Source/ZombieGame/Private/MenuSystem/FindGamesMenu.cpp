// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/FindGamesMenu.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "MenuSystem/ServerRow.h"
#include "Interfaces/ZINT_GameInstance.h"


UFindGamesMenu::UFindGamesMenu(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget>ServerRowClassWidgetBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if(!ServerRowClassWidgetBPClass.Class) return;
	ServerRowClass = ServerRowClassWidgetBPClass.Class;
}


bool UFindGamesMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(RefreshListButton!=nullptr)) return false;
	RefreshListButton->OnClicked.AddDynamic(this, &UFindGamesMenu::OnRefreshListButtonClicked);

	if(!ensure(JoinGameButton!=nullptr)) return false;
	JoinGameButton->OnClicked.AddDynamic(this, &UFindGamesMenu::OnJoinGameButtonClicked);

	return true;
}


void UFindGamesMenu::Setup(IZINT_GameInstance* NewInterface)
{
	Super::Setup(NewInterface);

	if(GameInstanceInterface != nullptr)
		GameInstanceInterface->SetFindGamesMenu(this);
	
	if(LoadingPopupUI != nullptr)
		LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);
}


void UFindGamesMenu::OnRefreshListButtonClicked()
{
	RefreshServerList();
}


void UFindGamesMenu::OnJoinGameButtonClicked()
{
	if(SelectedIndex.IsSet() && GameInstanceInterface != nullptr)
	{
		GameInstanceInterface->Join(SelectedIndex.GetValue());
	}
}


void UFindGamesMenu::UpdateAllRows()
{
	if(SelectedIndex.IsSet() && ServerList != nullptr)
	{
		for(int32 i = 0; i < ServerList->GetChildrenCount(); i++)
		{
			UServerRow* ServerRow = Cast<UServerRow>(ServerList->GetChildAt(i));
			if(ServerRow != nullptr)
			{
				ServerRow->SetSelected(SelectedIndex.GetValue() == i);
			}
		}
	}
}


void UFindGamesMenu::RefreshServerList()
{
	if(GameInstanceInterface != nullptr)
	{
		GameInstanceInterface->RefreshServerList();
	}
}


void UFindGamesMenu::SetServerList(TArray<FServerData> InServerList)
{
	if(ServerList != nullptr)
	{
		ClearServerList();
		if(ServerRowClass != nullptr)
		{
			uint32 itr = 0;
			for(auto ServerData : InServerList)
			{
				UServerRow* NewServerRow = CreateWidget<UServerRow>(this,ServerRowClass);

				if(NewServerRow != nullptr)
				{
					NewServerRow->Setup(this,itr,FText::FromString(ServerData.ServerName),
					FText::FromString(ServerData.HostUsername), ServerData.CurrentPlayers,
					ServerData.MaxPlayers);

					ServerList->AddChild(NewServerRow);
					itr++;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to create ServerRow widget from ServerRowBPClass"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerRowBPClass not set"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerList widget not found"));
	}
}


void UFindGamesMenu::ClearServerList()
{
	if(ServerList != nullptr)
		ServerList->ClearChildren();
	SelectedIndex.Reset();
}


void UFindGamesMenu::SetSelectedIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateAllRows();
}


void UFindGamesMenu::ToggleLoadingPopupUI(bool bVisible)
{
	if(LoadingPopupUI != nullptr)
	{
		if(bVisible)
			LoadingPopupUI->SetVisibility(ESlateVisibility::Visible);
		else
			LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);
	}		
}
