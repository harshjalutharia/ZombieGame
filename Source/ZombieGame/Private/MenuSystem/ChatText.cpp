// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/ChatText.h"
#include "Components/RichTextBlock.h"


void UChatText::SetMessage(const FString& PlayerName, const FString& Message, const EChatLogType ChatLogType)
{
	if(ChatMessage != nullptr)
	{
		switch(ChatLogType)
		{
			case EChatLogType::Default:
				ChatMessage->SetText(FText::FromString(FString::Printf(TEXT("<DefaultSubject>%s : </>%s"),*PlayerName, *Message)));
				break;
			
			case EChatLogType::Error:
				ChatMessage->SetText(FText::FromString(FString::Printf(TEXT("<ErrorSubject>%s </><Error>%s</>"),*PlayerName, *Message)));
				break;
			
			case EChatLogType::System:
				ChatMessage->SetText(FText::FromString(FString::Printf(TEXT("<SystemSubject>%s </><System>%s</>"),*PlayerName, *Message)));
				break;
				
			default: break;
		}
	}
}
