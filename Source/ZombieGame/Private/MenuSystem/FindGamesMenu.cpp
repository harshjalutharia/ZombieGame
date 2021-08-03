// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/FindGamesMenu.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "MenuSystem/ServerRow.h"
#include "Interfaces/ZINT_GameInstance.h"


UFindGamesMenu::UFindGamesMenu(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget>ServerRowClassWidgetBPClass(TEXT("/Game/MenuSystem/MainMenu/WBP_ServerRow"));
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

	if(!ensure(CancelButtonPasswordPopup!=nullptr)) return false;
	CancelButtonPasswordPopup->OnClicked.AddDynamic(this, &UFindGamesMenu::OnCancelButtonClicked);

	if(!ensure(JoinButtonPasswordPopup!=nullptr)) return false;
	JoinButtonPasswordPopup->OnClicked.AddDynamic(this, &UFindGamesMenu::OnPopupJoinButtonClicked);

	return true;
}


void UFindGamesMenu::Setup(IZINT_GameInstance* NewInterface)
{
	Super::Setup(NewInterface);

	if(GameInstanceInterface != nullptr)
		GameInstanceInterface->SetFindGamesMenu(this);
	
	if(LoadingPopupUI != nullptr)
		LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);

	if(PasswordPopupUI != nullptr)
		PasswordPopupUI->SetVisibility(ESlateVisibility::Hidden);

	if(WrongPasswordText != nullptr)
		WrongPasswordText->SetVisibility(ESlateVisibility::Hidden);

	if(EditPassword != nullptr)
		EditPassword->SetIsPassword(true);
}


void UFindGamesMenu::OnRefreshListButtonClicked()
{
	RefreshServerList();
}


void UFindGamesMenu::OnJoinGameButtonClicked()
{
	if(GameInstanceInterface != nullptr)
	{
		if(SelectedIndex.IsSet() && SelectedIndex.GetValue() < AllServerRows.Num())
		{
			if(AllServerRows[SelectedIndex.GetValue()]->GetIsPasswordProtected())
			{
				SelectedServerPassword = AllServerRows[SelectedIndex.GetValue()]->GetPassword();
				
				if(PasswordPopupUI != nullptr)
					PasswordPopupUI->SetVisibility(ESlateVisibility::Visible);
				if(WrongPasswordText != nullptr)
					WrongPasswordText->SetVisibility(ESlateVisibility::Hidden);
				if(EditPassword != nullptr)
					EditPassword->SetText(FText::FromString(""));
			}
			else
			{
				if(PasswordPopupUI != nullptr)
					PasswordPopupUI->SetVisibility(ESlateVisibility::Hidden);
				GameInstanceInterface->Join(SelectedIndex.GetValue());
			}
		}
	}
}


void UFindGamesMenu::OnCancelButtonClicked()
{
	if(PasswordPopupUI != nullptr)
		PasswordPopupUI->SetVisibility(ESlateVisibility::Hidden);
}


void UFindGamesMenu::OnPopupJoinButtonClicked()
{
	if(EditPassword != nullptr && GameInstanceInterface != nullptr)
	{
		if(SelectedServerPassword.Equals(EditPassword->GetText().ToString()))
		{
			if(PasswordPopupUI != nullptr)
				PasswordPopupUI->SetVisibility(ESlateVisibility::Hidden);
			GameInstanceInterface->Join(SelectedIndex.GetValue());
		}
		else
		{
			if(WrongPasswordText != nullptr)
				WrongPasswordText->SetVisibility(ESlateVisibility::Visible);
			EditPassword->SetText(FText::FromString(""));
		}
	}
}


void UFindGamesMenu::UpdateAllRows()
{
	if(SelectedIndex.IsSet() && AllServerRows.Num() > 0)
	{
		int32 i = 0;
		for(auto& ServerRow : AllServerRows)
		{
			ServerRow->SetSelected(SelectedIndex.GetValue() == i);
			i++;
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


void UFindGamesMenu::SetServerList(TArray<FLobbyServerInfo> InServerList)
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
					NewServerRow->Setup(this,itr,FText::FromString(ServerData.ServerName), ServerData.Password,
					FText::FromString(ServerData.GameModeName), FText::FromString(ServerData.MapName), ServerData.CurrentPlayers,
					ServerData.MaxPlayers);

					ServerList->AddChild(NewServerRow);
					AllServerRows.Add(NewServerRow);
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
	AllServerRows.Empty();

	UServerRow* ServerHeader = CreateWidget<UServerRow>(this,ServerRowClass);
	if(ServerHeader != nullptr)
	{
		ServerHeader->SetAsHeader();
		ServerList->AddChild(ServerHeader);
	}	

	if(PasswordPopupUI != nullptr)
		PasswordPopupUI->SetVisibility(ESlateVisibility::Hidden);
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
		{
			LoadingPopupUI->SetVisibility(ESlateVisibility::Visible);
			ClearServerList();
		}
		else
			LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);
	}		
}
