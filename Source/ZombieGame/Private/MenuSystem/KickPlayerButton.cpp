// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/KickPlayerButton.h"
#include "LobbyGameMode.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


bool UKickPlayerButton::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(PlayerButton!=nullptr)) return false;
	PlayerButton->OnClicked.AddDynamic(this, &UKickPlayerButton::OnPlayerButtonClicked);

	return true;
}


void UKickPlayerButton::OnPlayerButtonClicked()
{
	if(!GetWorld()->IsServer()) return;
	
	ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	LobbyGameMode->KickPlayer(PlayerInfo.PlayerID);
}


void UKickPlayerButton::SetPlayerInfo(const FLobbyPlayerInfo& InPlayerInfo)
{
	PlayerInfo = InPlayerInfo;
	if(PlayerName != nullptr)
	{
		PlayerName->SetText(FText::FromString(PlayerInfo.PlayerName));
	}
	if(PlayerInfo.PlayerAvatarImage != nullptr && AvatarImage != nullptr)
	{
		AvatarImage->SetBrushFromTexture(PlayerInfo.PlayerAvatarImage);
		AvatarImage->Brush.ImageSize = FVector2D(32.f,32.f);
	}
}
