// Fill out your copyright notice in the Description page of Project Settings.


#include "ZPlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.h"


AZPlayerCameraManager::AZPlayerCameraManager()
{
	NormalFOV = 90.f;
	AimingFOV = 65.f;
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
	APlayerCharacter* Player = Cast<APlayerCharacter>(PCOwner->GetPawn());
	if(Player)
	{
		const float TargetFOV = (Player->IsAiming())? AimingFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, FOVInterpSpeed);
		SetFOV(DefaultFOV);
		
		if(Player->bIsCrouched && !bWasCrouched)
		{
			CurrentCrouchOffset = MaxCrouchOffsetZ;
		}
		else if(!Player->bIsCrouched && bWasCrouched)
		{
			CurrentCrouchOffset = -MaxCrouchOffsetZ;
		}

		bWasCrouched = Player->bIsCrouched;
		CurrentCrouchOffset = FMath::LerpStable(CurrentCrouchOffset, 0.f, FMath::Clamp(CrouchLerpSpeed*DeltaTime,0.f, 1.f));

		const FVector CurrentCameraOffset = Player->GetCameraComponent()->GetRelativeLocation();
		const FVector NewCameraOffset = FVector(CurrentCameraOffset.X, CurrentCameraOffset.Y, DefaultCameraOffsetZ + CurrentCrouchOffset);
		Player->GetCameraComponent()->SetRelativeLocation(NewCameraOffset);
	}
	Super::UpdateCamera(DeltaTime);
}
