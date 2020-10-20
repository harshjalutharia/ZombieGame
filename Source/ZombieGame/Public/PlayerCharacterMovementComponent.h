// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ObjectMacros.h"

#include "PlayerCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UPlayerCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_Custom;

public:

	UPlayerCharacterMovementComponent();
	
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Custom Character Movement")
	float JogSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Character Movement")
	float AimWalkSpeed = 225.f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Character Movement")
	float SprintSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Character Movement")
	float CrouchSpeed = 200.f;

	uint8 WantsToSprint : 1;
	uint8 WantsToAimWalk : 1;

	bool SprintKeyDown = false;
	bool AimKeyDown = false;
	
	bool IsReloading = false;

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
	virtual float GetMaxSpeed() const override;

	void SetSprinting(bool Sprinting);
	
	void SetAiming(bool AimWalking);

	void SetReloading(bool Reloading); 

	/*
	Following two functions can be deleted if not being used in AnimBP
	*/
	UFUNCTION(BlueprintCallable)
	bool IsSprinting() const { return WantsToSprint; }

	UFUNCTION(BlueprintCallable)
	bool IsAiming() const { return AimKeyDown; }
	
};


class FSavedMove_Custom : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;

public:

	//FSavedMove_Custom();

	virtual void Clear() override;

	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const override;

	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void PrepMoveFor(class ACharacter* Character) override;

private:

	uint8 SavedWantsToSprint : 1;
	uint8 SavedWantsToAimWalk : 1;
};


class FNetworkPredictionData_Client_Custom : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	// Constructor
	FNetworkPredictionData_Client_Custom(const UCharacterMovementComponent& ClientMovement);

	//brief Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};