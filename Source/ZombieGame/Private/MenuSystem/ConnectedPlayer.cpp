// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/ConnectedPlayer.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UConnectedPlayer::SetPlayerNameAndAvatar(const FString& InPlayerName, UTexture2D* InAvatar)
{
	if(PlayerName != nullptr)
	{
		PlayerName->SetText(FText::FromString(InPlayerName));
	}
	if(InAvatar != nullptr && AvatarImage != nullptr)
	{
		AvatarImage->SetBrushFromTexture(InAvatar);
		AvatarImage->Brush.ImageSize = FVector2D(32.f,32.f);
	}
}


void UConnectedPlayer::SetPlayerStatus(bool bIsReady)
{
	if(bIsReady)
	{
		if(ReadyImage != nullptr && StatusImage != nullptr)
		{
			StatusImage->SetBrushFromTexture(ReadyImage);
			StatusImage->Brush.ImageSize = FVector2D(24.f,24.f);
		}
	}
	else
	{
		if(NotReadyImage != nullptr && StatusImage != nullptr)
		{
			StatusImage->SetBrushFromTexture(NotReadyImage);
			StatusImage->Brush.ImageSize = FVector2D(24.f,24.f);
		}
	}
}
