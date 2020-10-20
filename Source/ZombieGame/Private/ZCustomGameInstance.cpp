// Fill out your copyright notice in the Description page of Project Settings.


#include "ZCustomGameInstance.h"
#include "Blueprint/UserWidget.h"


UZCustomGameInstance::UZCustomGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget>PlayerHUDWidgetBPClass(TEXT("/Game/UI/WBP_PlayerUI"));
	if(!PlayerHUDWidgetBPClass.Class)
		return;
	PlayerHUDClass=PlayerHUDWidgetBPClass.Class;
}


void UZCustomGameInstance::LoadPlayerHUD()
{
	if(!PlayerHUDClass)
		return;
	
	APlayerController* PC = GetFirstLocalPlayerController(GetWorld());
	if(!PC)
		return;

	UUserWidget* PlayerHUD = CreateWidget(PC,PlayerHUDClass);
	if(!PlayerHUD)
		return;

	PlayerHUD->AddToViewport();
}
