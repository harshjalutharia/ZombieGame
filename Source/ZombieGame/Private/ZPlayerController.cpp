// Fill out your copyright notice in the Description page of Project Settings.


#include "ZPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "ZPlayerCameraManager.h"
#include "Interfaces/ZINT_GameInstance.h"


AZPlayerController::AZPlayerController()
{
    PlayerCameraManagerClass = AZPlayerCameraManager::StaticClass();
}


void AZPlayerController::AssignPlayerHUD_Implementation(UPlayerUI* NewHUD)
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
}


void AZPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    InputComponent->BindAction("PauseMenu",IE_Pressed, this, &AZPlayerController::ShowPauseMenu);
}


void AZPlayerController::ShowPauseMenu()
{
    if(GetGameInstance()->GetClass()->ImplementsInterface(UZINT_GameInstance::StaticClass()))
    {
        IZINT_GameInstance::Execute_ShowPauseMenu(GetGameInstance());
    }
}
