// Fill out your copyright notice in the Description page of Project Settings.


#include "ZPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "ZPlayerCameraManager.h"
#include "Interfaces/ZINT_PlayerHUD.h"


AZPlayerController::AZPlayerController()
{
    PlayerCameraManagerClass = AZPlayerCameraManager::StaticClass();
}


void AZPlayerController::AssignPlayerHUD_Implementation(UUserWidget* NewHUD)
{
    PlayerHUD = NewHUD;
}


void AZPlayerController::PlayLocalFiringEffects_Implementation(TSubclassOf<UMatineeCameraShake> FireCameraShake)
{
    ClientStartCameraShake(FireCameraShake);

    if(PlayerHUD && PlayerHUD->IsInViewport())
    {
        if(PlayerHUD->GetClass()->ImplementsInterface(UZINT_PlayerHUD::StaticClass()))
        {
            IZINT_PlayerHUD::Execute_PlayCrosshairRecoil(PlayerHUD);
        }
    }
}
