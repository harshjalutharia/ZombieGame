// Fill out your copyright notice in the Description page of Project Settings.


#include "ZPlayerController.h"

#include "ZPlayerCameraManager.h"


AZPlayerController::AZPlayerController()
{
    PlayerCameraManagerClass = AZPlayerCameraManager::StaticClass();
}
