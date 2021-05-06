// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/ZINT_ZPlayerController.h"
#include "ZPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API AZPlayerController : public APlayerController , public IZINT_ZPlayerController
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	UUserWidget* PlayerHUD;

public:

	AZPlayerController();

	virtual void AssignPlayerHUD_Implementation(UUserWidget* NewHUD) override;

	virtual void PlayLocalFiringEffects_Implementation(TSubclassOf<UMatineeCameraShake> FireCameraShake) override;

};
