// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/HostMenu.h"
#include "MenuSystem/HorizontalList.h"
#include "Components/EditableText.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "Interfaces/ZINT_GameInstance.h"


bool UHostMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(HostButtonHostMenu!=nullptr)) return false;
	HostButtonHostMenu->OnClicked.AddDynamic(this, &UHostMenu::OnHostButtonHostMenuClicked);
	
	if(EditPassword != nullptr)
		EditPassword->SetIsPassword(true);

	return true;
}


void UHostMenu::OnHostButtonHostMenuClicked()
{
	if(GameInstanceInterface != nullptr)
	{
		FString ServerName = "";
		if(EditServerName != nullptr)
			ServerName = EditServerName->GetText().ToString();
		ServerName = (ServerName == "")? "My Server" : ServerName;

		FString Password = "";
		if(EditPassword != nullptr)
			Password = EditPassword->GetText().ToString();

		const uint8 MapIndex = HLS_Map->GetCurrentIndex();
		const uint8 GameModeIndex = HLS_GameMode->GetCurrentIndex();
		const uint8 MaxPlayers = HLS_MaxPlayers->GetCurrentIndex() + 1;

		bool bLanMatch = false;
		if(LANCheckBox != nullptr)
			bLanMatch = LANCheckBox->IsChecked();
		
		GameInstanceInterface->Host(ServerName, Password, GameModeIndex, MapIndex, MaxPlayers, bLanMatch);
	}
}


void UHostMenu::Setup(IZINT_GameInstance* NewInterface)
{
	Super::Setup(NewInterface);

	if(GameInstanceInterface != nullptr)
	{
		GameInstanceInterface->GetAllGameModesAndMaps(HLS_GameMode->List,HLS_Map->List);
		const uint8 MaxPlayersAllowed = GameInstanceInterface->GetMaxAllowedPlayers();
		for(uint8 i = 1; i <= MaxPlayersAllowed; i++)
		{
			HLS_MaxPlayers->List.Add(FString::FromInt(i));
		}

		HLS_MaxPlayers->Setup();
		HLS_Map->Setup();
		HLS_GameMode->Setup();
	}
}
