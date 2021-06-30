// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/ChatText.h"
#include "Components/TextBlock.h"


void UChatText::SetMessage(const FString& Message)
{
	if(ChatMessage != nullptr)
	{
		ChatMessage->SetText(FText::FromString(Message));
	}
}
