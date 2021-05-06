// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Interfaces/ZINT_GameInstance.h"
#include "MenuSystem/ServerRow.h"


bool UMainMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(ErrorPopupButton!=nullptr)) return false;
	ErrorPopupButton->OnClicked.AddDynamic(this, &UMainMenu::OnPopupButtonClicked);

	if(!ensure(HostButton!=nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostClicked);

	if(!ensure(JoinButton!=nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinClicked);

	if(!ensure(QuitButton!=nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::ExitGame);
	
	if(!ensure(BackButtonHostMenu != nullptr)) return false;
	BackButtonHostMenu->OnClicked.AddDynamic(this, &UMainMenu::OnBackHostClicked);
	
	if(!ensure(HostButtonHostMenu != nullptr)) return false;
    HostButtonHostMenu->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if(!ensure(BackButtonJoinMenu != nullptr)) return false;
	BackButtonJoinMenu->OnClicked.AddDynamic(this, &UMainMenu::OnBackJoinClicked);

	if(!ensure(RefreshListButton != nullptr)) return false;
	RefreshListButton->OnClicked.AddDynamic(this, &UMainMenu::OnRefreshListClicked);

	if(!ensure(JoinButtonJoinMenu != nullptr)) return false;
	JoinButtonJoinMenu->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if(ErrorPopupUI != nullptr)
		ErrorPopupUI->SetVisibility(ESlateVisibility::Hidden);

	if(LoadingPopupUI != nullptr)
		LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);

	if(MenuSwitcher != nullptr && MainMenu != nullptr)
		MenuSwitcher->SetActiveWidget(MainMenu);
	
	return true;
}


void UMainMenu::ExitGame()
{
	auto PC = GetOwningPlayer();
	if(PC!=nullptr)
		PC->ConsoleCommand("Quit");
}


void UMainMenu::OnPopupButtonClicked()
{
	if(ErrorPopupUI != nullptr)
		ErrorPopupUI->SetVisibility(ESlateVisibility::Hidden);
}


void UMainMenu::ShowErrorMessage(FString ErrorMessage)
{
	if(LoadingPopupUI != nullptr)
		LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);
	
	if(ErrorPopupUI != nullptr)
	{
		ErrorPopupUI->SetVisibility(ESlateVisibility::Visible);
		
		if(ErrorPopupText != nullptr)
			ErrorPopupText->SetText(FText::FromString(ErrorMessage));
	}
}


void UMainMenu::ShowLoadingMessage(FString LoadingMessage)
{
	if(LoadingPopupUI != nullptr)
	{
		LoadingPopupUI->SetVisibility(ESlateVisibility::Visible);

		if(LoadingPopupText != nullptr)
			LoadingPopupText->SetText(FText::FromString(LoadingMessage));
	}
}


void UMainMenu::StopShowingLoadingMessage()
{
	if(LoadingPopupUI != nullptr)
		LoadingPopupUI->SetVisibility(ESlateVisibility::Hidden);
}


void UMainMenu::OnHostClicked()
{
	if(MenuSwitcher!=nullptr && HostMenu!=nullptr)
		MenuSwitcher->SetActiveWidget(HostMenu);
}


void UMainMenu::OnJoinClicked()
{
	if(MenuSwitcher!=nullptr && JoinMenu!=nullptr)
	{
		MenuSwitcher->SetActiveWidget(JoinMenu);
		
		GameInstanceInterface->RefreshServerList();
	}
}


void UMainMenu::OnBackHostClicked()
{
	if(MenuSwitcher!=nullptr && MainMenu!=nullptr)
		MenuSwitcher->SetActiveWidget(MainMenu);
}


void UMainMenu::HostServer()
{
	if(GameInstanceInterface != nullptr)
	{
		if(HostButtonHostMenu != nullptr)
			HostButton->SetIsEnabled(false);
		
		FString ServerName = "";
		if(EditServerName != nullptr)
			ServerName = EditServerName->GetText().ToString();
		ServerName = (ServerName == "")? "My Server" : ServerName;
		GameInstanceInterface->Host(ServerName);
	}
}


void UMainMenu::OnBackJoinClicked()
{
	if(MenuSwitcher!=nullptr && MainMenu!=nullptr)
		MenuSwitcher->SetActiveWidget(MainMenu);

	if(GameInstanceInterface != nullptr)
		GameInstanceInterface->CancelServerSearch();
}


void UMainMenu::OnRefreshListClicked()
{
	if(GameInstanceInterface != nullptr)
	{
		GameInstanceInterface->RefreshServerList();
	}
}


void UMainMenu::JoinServer()
{
	if(SelectedIndex.IsSet() && GameInstanceInterface != nullptr)
	{
		GameInstanceInterface->Join(SelectedIndex.GetValue());
	}
}


void UMainMenu::UpdateAllRows()
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


void UMainMenu::SetServerList(TArray<FServerData> InServerList)
{
	if(ServerList != nullptr)
	{
		ServerList->ClearChildren();
		if(ServerRowBPClass != nullptr)
		{
			uint32 itr = 0;
			for(auto ServerData : InServerList)
			{
				UServerRow* NewServerRow = CreateWidget<UServerRow>(this,ServerRowBPClass);

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


void UMainMenu::ClearServerList()
{
	if(ServerList != nullptr)
		ServerList->ClearChildren();
}


void UMainMenu::SetSelectedIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateAllRows();
}

