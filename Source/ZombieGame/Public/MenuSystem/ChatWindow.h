// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "ZombieGame/ZEnums.h"
#include "ChatWindow.generated.h"

class UEditableTextBox;
class UScrollBox;
class ALobbyPlayerController;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UChatWindow : public UMenuWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

private:
	
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ChatEntry;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ChatLog;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UChatText> ChatTextWidgetClass;

	UPROPERTY()
	ALobbyPlayerController* LobbyPCRef;

	UFUNCTION()
	void OnChatEntryCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void SetChatBoxFocus();
	
public:

	virtual void Setup(IZINT_GameInstance* NewInterface) override;

	void SetLobbyPlayerControllerReference(ALobbyPlayerController* InPlayerController) { LobbyPCRef = InPlayerController; }

	void AddMessageToChatWindow(const FString& PlayerName, const FString& Message, const EChatLogType ChatLogType);
};
