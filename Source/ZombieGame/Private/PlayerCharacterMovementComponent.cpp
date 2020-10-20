// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterMovementComponent.h"

#include "PlayerCharacter.h"


UPlayerCharacterMovementComponent::UPlayerCharacterMovementComponent()
{
	GravityScale = 1.5f;
	JumpZVelocity = 500;
	bCanWalkOffLedges = true;
	bCanWalkOffLedgesWhenCrouching = true;
}


FNetworkPredictionData_Client* UPlayerCharacterMovementComponent::GetPredictionData_Client() const
{
	if(ClientPredictionData == nullptr)
	{
		UPlayerCharacterMovementComponent* MutableThis = const_cast<UPlayerCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Custom(*this);
	}
	return ClientPredictionData;
}


void UPlayerCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (GetPawnOwner()->IsLocallyControlled())
	{
		if(AimKeyDown && !IsCrouching())
		{
			WantsToAimWalk = true;
		}
		else
		{
			WantsToAimWalk = false;
		}

		if (SprintKeyDown && !IsCrouching() && !WantsToAimWalk && !IsReloading)
		{
			APawn* Player = GetPawnOwner();
			const FVector PlayerVelocity = Player->GetVelocity().GetSafeNormal2D();
			const FVector PlayerOrientation = Player->GetActorForwardVector().GetSafeNormal2D();
			WantsToSprint = (FVector::DotProduct(PlayerVelocity, PlayerOrientation) > 0.8);
		}
		else
		{
			WantsToSprint = false;
		}
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UPlayerCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	WantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	WantsToAimWalk = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}


float UPlayerCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	switch(MovementMode)
	{
	case MOVE_Walking:
    case MOVE_NavWalking:
		{
			if(IsCrouching())
				return CrouchSpeed;
			if(WantsToAimWalk)
				return AimWalkSpeed;
			if(WantsToSprint)
				return SprintSpeed;
			return JogSpeed;
		}
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
        return 0.f;
	}
}


void UPlayerCharacterMovementComponent::SetSprinting(bool Sprinting)
{
	SprintKeyDown = Sprinting;
}


void UPlayerCharacterMovementComponent::SetAiming(bool AimWalking)
{
	AimKeyDown = AimWalking;
}


void UPlayerCharacterMovementComponent::SetReloading(bool Reloading)
{
	IsReloading = Reloading;
}


void FSavedMove_Custom::Clear()
{
	Super::Clear();

	SavedWantsToSprint = 0;
	SavedWantsToAimWalk = 0;
}


uint8 FSavedMove_Custom::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if(SavedWantsToSprint)
		Result |= FLAG_Custom_0;
	if(SavedWantsToAimWalk)
		Result |= FLAG_Custom_1;

	return Result;
}

bool FSavedMove_Custom::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_Custom* NewMove = static_cast<FSavedMove_Custom*>(NewMovePtr.Get());

	if(SavedWantsToSprint != NewMove->SavedWantsToSprint || SavedWantsToAimWalk != NewMove->SavedWantsToAimWalk)
	{
		return false;
	}

	return Super::CanCombineWith(NewMovePtr, Character, MaxDelta);
}

void FSavedMove_Custom::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UPlayerCharacterMovementComponent* MoveComp = static_cast<UPlayerCharacterMovementComponent*>(Character->GetCharacterMovement());
	if(MoveComp)
	{
		SavedWantsToSprint = MoveComp->WantsToSprint;
		SavedWantsToAimWalk = MoveComp->WantsToAimWalk;
	}
}


void FSavedMove_Custom::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UPlayerCharacterMovementComponent* MoveComp = Cast<UPlayerCharacterMovementComponent>(Character->GetCharacterMovement());
	if(MoveComp)
	{
		MoveComp->WantsToSprint = SavedWantsToSprint;
		MoveComp->WantsToAimWalk = SavedWantsToAimWalk;
	}
}


FNetworkPredictionData_Client_Custom::FNetworkPredictionData_Client_Custom(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}


FSavedMovePtr FNetworkPredictionData_Client_Custom::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Custom());
}


