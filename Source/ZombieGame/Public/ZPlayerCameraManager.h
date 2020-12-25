// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ZPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API AZPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	AZPlayerCameraManager();
	
	virtual void UpdateCamera(float DeltaTime) override;

	float NormalFOV;
	float FOVInterpSpeed;

	float CurrentCrouchOffset;
	float DefaultCameraOffsetZ;
	float MaxCrouchOffsetZ;
	float CrouchLerpSpeed;
	bool bWasCrouched;
};
