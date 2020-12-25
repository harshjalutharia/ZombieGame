// Fill out your copyright notice in the Description page of Project Settings.


#include "ZPlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Interfaces/ZINT_PlayerCharacter.h"


AZPlayerCameraManager::AZPlayerCameraManager()
{
	NormalFOV = 90.f;
	FOVInterpSpeed = 20.f;

	ViewPitchMax = 87.f;
	ViewPitchMin = -80.f;
	bAlwaysApplyModifiers = true;

	DefaultCameraOffsetZ = 0.f;
	MaxCrouchOffsetZ = 35.f;
	CrouchLerpSpeed = 12.f;
}


void AZPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	const APawn* Player = PCOwner->GetPawn();
	if(Player && Player->GetClass()->ImplementsInterface(UZINT_PlayerCharacter::StaticClass()))
	{
		const float TargetFOV = (IZINT_PlayerCharacter::Execute_GetIsAiming(Player))? IZINT_PlayerCharacter::Execute_GetWeaponZoomFOV(Player) : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, FOVInterpSpeed);
		SetFOV(DefaultFOV);
		
		if(IZINT_PlayerCharacter::Execute_GetIsCrouched(Player) && !bWasCrouched)
		{
			CurrentCrouchOffset = MaxCrouchOffsetZ;
		}
		else if(!IZINT_PlayerCharacter::Execute_GetIsCrouched(Player) && bWasCrouched)
		{
			CurrentCrouchOffset = -MaxCrouchOffsetZ;
		}

		bWasCrouched = IZINT_PlayerCharacter::Execute_GetIsCrouched(Player);
		CurrentCrouchOffset = FMath::LerpStable(CurrentCrouchOffset, 0.f, FMath::Clamp(CrouchLerpSpeed*DeltaTime,0.f, 1.f));

		const FVector CurrentCameraOffset = IZINT_PlayerCharacter::Execute_GetCameraComponent(Player)->GetRelativeLocation();
		const FVector NewCameraOffset = FVector(CurrentCameraOffset.X, CurrentCameraOffset.Y, DefaultCameraOffsetZ + CurrentCrouchOffset);
		IZINT_PlayerCharacter::Execute_GetCameraComponent(Player)->SetRelativeLocation(NewCameraOffset);
	}
	Super::UpdateCamera(DeltaTime);
}
