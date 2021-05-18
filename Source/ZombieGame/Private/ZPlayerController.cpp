// Fill out your copyright notice in the Description page of Project Settings.


#include "ZPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "ZPlayerCameraManager.h"
#include "Interfaces/ZINT_PlayerHUD.h"
#include "ZCustomGameInstance.h"


AZPlayerController::AZPlayerController()
{
    PlayerCameraManagerClass = AZPlayerCameraManager::StaticClass();
}


void AZPlayerController::AssignPlayerHUD(IZINT_PlayerHUD* NewHUD)
{
    PlayerHUD = NewHUD;
}


void AZPlayerController::PlayLocalFiringEffects_Implementation(TSubclassOf<UMatineeCameraShake> FireCameraShake)
{
    ClientStartCameraShake(FireCameraShake);

    if(PlayerHUD != nullptr)
    {
        PlayerHUD->PlayCrosshairRecoil();
    }
}


void AZPlayerController::BeginPlay()
{
    Super::BeginPlay();

    GameInstance = GetGameInstance<UZCustomGameInstance>();
    GameInstance->SetPlayerControllerInterface(this);
}


void AZPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    InputComponent->BindAction("PauseMenu",IE_Pressed, this, &AZPlayerController::ShowPauseMenu);
}


void AZPlayerController::ShowPauseMenu()
{
    if(GameInstance != nullptr)
    {
        GameInstance->ShowPauseMenu();
    }
}
