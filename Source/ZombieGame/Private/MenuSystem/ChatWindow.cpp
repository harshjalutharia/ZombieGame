// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/ChatWindow.h"
#include "LobbyPlayerController.h"
#include "Types/SlateEnums.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "MenuSystem/ChatText.h"


bool UChatWindow::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success) return false;

	if(ChatEntry != nullptr)
		ChatEntry->OnTextCommitted.AddDynamic(this, &UChatWindow::OnChatEntryCommitted);
	
	else
		return false;
	
	return true;
}


void UChatWindow::OnChatEntryCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(LobbyPCRef != nullptr && CommitMethod==ETextCommit::OnEnter)
	{
		LobbyPCRef->Server_SendChatMessage(Text.ToString());
		if(ChatEntry != nullptr)
		{
			ChatEntry->SetText(FText::FromString(""));
			FTimerHandle TempHandle;
			GetWorld()->GetTimerManager().SetTimer(TempHandle,this, &UChatWindow::SetChatBoxFocus, 0.1f,false);
		}
	}
}


void UChatWindow::SetChatBoxFocus()
{
	if(ChatEntry != nullptr)
		ChatEntry->SetFocus();
}


void UChatWindow::Setup(IZINT_GameInstance* NewInterface)
{
	Super::Setup(NewInterface);

	ChatLog->ClearChildren();
}


void UChatWindow::AddMessageToChatWindow(const FString& PlayerName, const FString& Message, const EChatLogType ChatLogType)
{
	if(ChatLog != nullptr && ChatTextWidgetClass != nullptr)
	{
		UChatText* NewText = CreateWidget<UChatText>(this, ChatTextWidgetClass);
		if(NewText != nullptr)
		{
			NewText->SetMessage(PlayerName,Message,ChatLogType);
			ChatLog->AddChild(NewText);
			ChatLog->ScrollToEnd();
		}
	}
}
