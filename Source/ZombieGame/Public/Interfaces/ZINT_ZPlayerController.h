// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ZINT_ZPlayerController.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UZINT_ZPlayerController : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIEGAME_API IZINT_ZPlayerController
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void AssignPlayerHUD(UUserWidget* NewHUD);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlayLocalFiringEffects(TSubclassOf<UCameraShake> FireCameraShake);
};
