// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ZINT_PlayerCharacter.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UZINT_PlayerCharacter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIEGAME_API IZINT_PlayerCharacter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	float GetWeaponZoomFOV() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsAiming() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetIsCrouched() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    class UCameraComponent* GetCameraComponent() const;
};
