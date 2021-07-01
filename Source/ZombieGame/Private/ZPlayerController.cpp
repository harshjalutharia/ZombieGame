// Fill out your copyright notice in the Description page of Project Settings.


#include "ZPlayerController.h"

#include "PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "ZPlayerCameraManager.h"
#include "Interfaces/ZINT_GameInstance.h"


AZPlayerController::AZPlayerController()
{
	PlayerCameraManagerClass = AZPlayerCameraManager::StaticClass();

	bIsPauseMenuActive = false;
}


void AZPlayerController::AssignPlayerHUD_Implementation(UPlayerUI* NewHUD)
{
	PlayerHUD = NewHUD;
}


void AZPlayerController::PlayLocalFiringEffects_Implementation(TSubclassOf<UMatineeCameraShake> FireCameraShake)
{
	ClientStartCameraShake(FireCameraShake);

	if (PlayerHUD != nullptr)
	{
		PlayerHUD->PlayCrosshairRecoil();
	}
}


void AZPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetGameInstance()->GetClass()->ImplementsInterface(UZINT_GameInstance::StaticClass()))
	{
		PauseMenu = IZINT_GameInstance::Execute_LoadPauseMenu(GetGameInstance());
	}
}


void AZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &AZPlayerController::TogglePauseMenu);
}


void AZPlayerController::TogglePauseMenu()
{
	if (PauseMenu != nullptr)
	{
		if (bIsPauseMenuActive)
		{
			PauseMenu->HidePauseMenu();
			bIsPauseMenuActive = false;
		}
		else
		{
			PauseMenu->ShowPauseMenu();
			bIsPauseMenuActive = true;
		}

		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
		if(PlayerCharacter)
			PlayerCharacter->SetIsPauseMenuActive(bIsPauseMenuActive);
	}
}
