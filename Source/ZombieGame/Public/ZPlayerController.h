// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/ZINT_ZPlayerController.h"
#include "MenuSystem/PauseMenu.h"

#include "ZPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API AZPlayerController : public APlayerController , public IZINT_ZPlayerController
{
	GENERATED_BODY()

	UPROPERTY()
	UPlayerUI* PlayerHUD;

public:

	AZPlayerController();

	virtual void AssignPlayerHUD_Implementation(UPlayerUI* NewHUD) override;

	virtual void PlayLocalFiringEffects_Implementation(TSubclassOf<UMatineeCameraShake> FireCameraShake) override;

protected:

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:
	
	UPROPERTY()
	class UPauseMenu* PauseMenu;

	bool bIsPauseMenuActive;

public:
	
	void TogglePauseMenu();

};
