// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieGame/Public/PlayerCharacter.h"
#include "ZombieGame/Public/PlayerCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ZHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "ZWeapon.h"


// Sets default values
APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MoveComp = Cast<UPlayerCharacterMovementComponent>(GetCharacterMovement());
	if(MoveComp)
	{
		MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
		MoveComp->SetJumpAllowed(true);
	}

	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	if(CameraBoomComp)
	{
		CameraBoomComp->bUsePawnControlRotation = true;
		CameraBoomComp->SocketOffset = FVector(0, 35, 0);
		CameraBoomComp->TargetOffset = FVector(0, 0, 55);
		CameraBoomComp->SetupAttachment(RootComponent);
	}

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	if(CameraComp)
	{
		CameraComp->SetupAttachment(CameraBoomComp);
	}

	HealthComp = CreateDefaultSubobject<UZHealthComponent>("HealthComp");
	if(HealthComp)
	{
		HealthComp->SetIsReplicated(true);
	}

	ActiveWeaponSocketName = TEXT("WeaponHand");
	ReserveWeaponSlot1SocketName = TEXT("ReserveWeaponOne");
	ReserveWeaponSlot2SocketName = TEXT("ReserveWeaponTwo");

	InteractionRange = 300.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		
		if(bSpawnWithWeapon && SpawnWithWeaponClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Instigator = this;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AZWeapon* NewWeapon = GetWorld()->SpawnActor<AZWeapon>(SpawnWithWeaponClass,FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
			if(NewWeapon)
			{
				AssignWeaponToSlot(NewWeapon, EInventorySlot::Weapon1);
				NewWeapon->SetEquipped(true);
				NewWeapon->WeaponPickedUp(this,EInventorySlot::Weapon1);
				ActiveWeapon = NewWeapon;
				NewWeapon->AttachWeaponToAssignedSlotOrHand(true);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Weapon Spawn for %s failed"), *GetName())
				ActiveWeapon = nullptr;
			}
				
			/*AZWeapon* NewWeapon2 = GetWorld()->SpawnActor<AZWeapon>(SpawnWithWeaponClass,FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
			if(NewWeapon2)
			{
				AssignWeaponToSlot(NewWeapon2, EInventorySlot::Weapon2);
				NewWeapon2->WeaponPickedUp(this, EInventorySlot::Weapon2);
			}*/
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon Class used for spawning default weapon not specified in blueprint."))
			ActiveWeapon = nullptr;
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsLocallyControlled() || HasAuthority())
		SetLookPitch();

	/*
	if(IsLocallyControlled())
	{
		const AActor* ActorBeingLookedAt = GetActorInView();
		
		if(ActorBeingLookedAt->GetClass()->ImplementsInterface(UZINT_InteractableActor::StaticClass()))
		{
			if(FocusedActor)
			{
				if(FocusedActor!=ActorBeingLookedAt)
				{
					if(IZINT_InteractableActor::Execute_ShouldHighlightWhenLookedAt(FocusedActor))
						IZINT_InteractableActor::Execute_StopHighlightingActor(FocusedActor);
					if(IZINT_InteractableActor::Execute_ShouldHighlightWhenLookedAt(ActorBeingLookedAt))
						IZINT_InteractableActor::Execute_StartHighlightingActor(ActorBeingLookedAt);
					FocusedActor=InteractableActor;
				}
			}
			else
			{
				if(IZINT_InteractableActor::Execute_ShouldHighlightWhenLookedAt(ActorBeingLookedAt))
					IZINT_InteractableActor::Execute_StartHighlightingActor(ActorBeingLookedAt);
				FocusedActor=InteractableActor;
			}
		}
		else if(FocusedActor)
		{
			if(IZINT_InteractableActor::Execute_ShouldHighlightWhenLookedAt(FocusedActor))
				IZINT_InteractableActor::Execute_StopHighlightingActor(FocusedActor);
			FocusedActor=nullptr;
		}
	}
	*/
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward",this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight",this, &APlayerCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch",IE_Pressed, this, &APlayerCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch",IE_Released, this, &APlayerCharacter::StopCrouching);

	PlayerInputComponent->BindAction("Sprint",IE_Pressed, this, &APlayerCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint",IE_Released, this, &APlayerCharacter::StopSprinting);

	PlayerInputComponent->BindAction("ADS",IE_Pressed, this, &APlayerCharacter::StartAiming);
	PlayerInputComponent->BindAction("ADS",IE_Released, this, &APlayerCharacter::StopAiming);

	PlayerInputComponent->BindAction("Fire",IE_Pressed, this, &APlayerCharacter::StartFiring);
	PlayerInputComponent->BindAction("Fire",IE_Released, this, &APlayerCharacter::StopFiring);

	PlayerInputComponent->BindAction("Jump",IE_Pressed, this, &APlayerCharacter::RequestJump);

	PlayerInputComponent->BindAction("Reload",IE_Pressed, this, &APlayerCharacter::RequestReload);

	PlayerInputComponent->BindAction("LastWeapon",IE_Pressed, this, &APlayerCharacter::QuickSwitchWeapon);

	
	PlayerInputComponent->BindAction("WeaponSlot1",IE_Pressed, this, &APlayerCharacter::UseWeaponSlot1);
	PlayerInputComponent->BindAction("WeaponSlot2",IE_Pressed, this, &APlayerCharacter::UseWeaponSlot2);

	PlayerInputComponent->BindAction("Interact",IE_Pressed, this, &APlayerCharacter::Interact);
}


bool APlayerCharacter::IsSprinting() const
{
	// Ensure that changes made here are also made in Tick function of UPlayerCharacterMovementComponent
	return (!bIsAiming && !bIsCrouched && ActiveWeaponState==EWeaponState::Idle && bWantsToSprint && 
		(FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorForwardVector().GetSafeNormal2D()) > 0.8));
}


void APlayerCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector()*Value);
}


void APlayerCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector()*Value);
}


void APlayerCharacter::StartCrouching()
{
	SetCrouching(true);
}


void APlayerCharacter::StopCrouching()
{
	SetCrouching(false);
}


void APlayerCharacter::StartSprinting()
{
	SetSprinting(true);
}


void APlayerCharacter::StopSprinting()
{
	SetSprinting(false);
}


void APlayerCharacter::StartAiming()
{
	if(HoldingWeapon())
		SetAiming(true);
}


void APlayerCharacter::StopAiming()
{
	if(HoldingWeapon())
		SetAiming(false);
}


void APlayerCharacter::RequestJump()
{
	SetJumping(true);
}


void APlayerCharacter::StartFiring()
{
	bWantsToFire = true;

	if(IsSprinting())
		return;
	
	if(ActiveWeapon)
	{
		ActiveWeapon->StartFiring();
	}
}


void APlayerCharacter::StopFiring()
{
	bWantsToFire = false;

	if(IsSprinting())
		return;
	
	if(ActiveWeapon)
	{
		ActiveWeapon->StopFiring();
	}
}


void APlayerCharacter::RequestReload()
{
	if(ActiveWeapon)
	{
		ActiveWeapon->ReloadWeapon();
	}
}


void APlayerCharacter::QuickSwitchWeapon()
{
	
}


void APlayerCharacter::UseWeaponSlot1()
{
	if(WeaponSlot1 != nullptr && !bWantsToFire && ActiveWeapon!=WeaponSlot1)
	{
		if(ActiveWeapon->IsReloading())
		{
			ActiveWeapon->InterruptReload();
		}
		ServerSwitchWeapon(WeaponSlot1);
	}
}


void APlayerCharacter::UseWeaponSlot2()
{
	if(WeaponSlot2 != nullptr && !bWantsToFire && ActiveWeapon!=WeaponSlot2)
	{
		if(ActiveWeapon->IsReloading())
		{
			ActiveWeapon->InterruptReload();
		}
		ServerSwitchWeapon(WeaponSlot2);
	}
}


void APlayerCharacter::Interact()
{
	if(ActiveWeaponState != EWeaponState::Idle && ActiveWeaponState != EWeaponState::Reloading)
		return;
	
	AActor* ActorBeingLookedAt = GetActorInView();

	if(ActorBeingLookedAt->GetClass()->ImplementsInterface(UZINT_InteractableActor::StaticClass()))
	{
		IZINT_InteractableActor::Execute_Interact(ActorBeingLookedAt,this);
		//UE_LOG(LogTemp, Warning, TEXT("Local, Actor valid"));
	}
	//else
		//UE_LOG(LogTemp, Warning, TEXT("Interact failed"));
}


AActor* APlayerCharacter::GetActorInView() const
{
	FVector CameraLocation;
	FRotator CameraRotation;

	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector StartLineTrace = CameraLocation;
	const FVector EndLineTrace = CameraLocation + CameraRotation.Vector() * InteractionRange;
	
	FHitResult OutHit;

	if(GetWorld()->LineTraceSingleByChannel(OutHit,StartLineTrace,EndLineTrace,ECC_Visibility))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Local, Interaction: Hit %s"),*OutHit.GetActor()->GetName());
		return OutHit.GetActor();
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Local, Interaction: No Hit"));
		return nullptr;
	}
}


void APlayerCharacter::SetLookPitch()
{
	const FRotator Look = GetControlRotation()-GetActorRotation();
	LookPitch = FRotator::NormalizeAxis(Look.Pitch);
}


void APlayerCharacter::SetCrouching(bool NewCrouching)
{
	if(IsJumping())
		return;
	
	if (NewCrouching)
	{
		ACharacter::Crouch();
	}
	else
	{
		ACharacter::UnCrouch();
	}
}


void APlayerCharacter::SetSprinting(bool NewSprinting)
{
	if(IsJumping())
		return;
	
	if(!HasAuthority())
	{
		ServerSetSprinting(NewSprinting);
	}
	bWantsToSprint = NewSprinting;
	
	MoveComp->SetSprinting(NewSprinting);

	if(!NewSprinting)
	{
		if(bWantsToFire)
			StartFiring();
	}
}


void APlayerCharacter::SetAiming(bool NewAiming)
{
	if(IsJumping() && NewAiming)
		return;
	
	if(!HasAuthority())
	{
		ServerSetAiming(NewAiming);
	}
	bIsAiming = NewAiming;
	
	MoveComp->SetAiming(NewAiming);
}


void APlayerCharacter::SetJumping(bool NewJumping)
{
	if(NewJumping)
	{
		if(!bIsCrouched && !bIsJumping)/*&& (
			(FVector::DotProduct(GetVelocity().GetSafeNormal2D(),GetActorForwardVector().GetSafeNormal2D())>0.8) ||
			(FVector::DotProduct(GetVelocity().GetSafeNormal2D(),GetActorForwardVector().GetSafeNormal2D())<-0.8) ||
			GetVelocity().IsZero()
			))*/
		{
			bIsJumping = true;
			
			if(GetVelocity().IsZero())
			{
				bIdleJump = true;
			}
			else
			{
				bIdleJump = false;
			}
			
			if(!HasAuthority())
			{
				ServerSetJumping(true,bIdleJump);
			}
			
			Jump();
		}
	}
	else
	{
		if(!HasAuthority())
		{
			ServerSetJumping(false);
		}
		bIsJumping = false;
	}
}


void APlayerCharacter::ServerSetSprinting_Implementation(bool NewSprinting)
{
	bWantsToSprint = NewSprinting;
}


bool APlayerCharacter::ServerSetSprinting_Validate(bool NewSprinting)
{
	return true;
}


void APlayerCharacter::ServerSetAiming_Implementation(bool NewAiming)
{
	bIsAiming = NewAiming;
}


bool APlayerCharacter::ServerSetAiming_Validate(bool NewAiming)
{
	return true;
}


void APlayerCharacter::ServerSetJumping_Implementation(bool NewJumping, bool IsIdleJump)
{
	bIsJumping = NewJumping;
	if(bIdleJump != IsIdleJump)
		bIdleJump = IsIdleJump;
}


bool APlayerCharacter::ServerSetJumping_Validate(bool NewJumping, bool IsIdleJump)
{
	return true;
}


void APlayerCharacter::ServerSwitchWeapon_Implementation(AZWeapon* NewActiveWeapon)
{
	LastWeaponInHand = ActiveWeapon;
	ActiveWeapon=NewActiveWeapon;
	ActiveWeapon->PullOutWeapon();
}


bool APlayerCharacter::ServerSwitchWeapon_Validate(AZWeapon* NewActiveWeapon)
{
	return true;
}


AZWeapon* APlayerCharacter::GetActiveWeapon() const
{
	return ActiveWeapon;
}


void APlayerCharacter::OnRep_ActiveWeapon()
{
	if(LastWeaponInHand!=nullptr)
	{
		// Switch weapons
		ActiveWeapon->PullOutWeapon();
	}
	else
	{
		// Only triggered when character spawns
		ActiveWeapon->AttachWeaponToAssignedSlotOrHand(true);
	}
}


EInventorySlot APlayerCharacter::GetFirstEmptyWeaponSlot() const
{
	if(WeaponSlot1 == nullptr)
		return EInventorySlot::Weapon1;
	if(WeaponSlot2 == nullptr)
		return EInventorySlot::Weapon2;
	
	return EInventorySlot::INVALID_SLOT;
}


FName APlayerCharacter::GetAttachPointOfSlot(EInventorySlot Slot) const
{
	if(Slot == EInventorySlot::Weapon1)
		return ReserveWeaponSlot1SocketName;
	
	if (Slot == EInventorySlot::Weapon2)
		return ReserveWeaponSlot2SocketName;
	
	return "";
}


void APlayerCharacter::AssignWeaponToSlot(AZWeapon* WeaponToAssign, EInventorySlot Slot)
{
	if(!HasAuthority())
		return;
	
	if(Slot == EInventorySlot::Weapon1)
	{
		WeaponSlot1=WeaponToAssign;
	}
	else if (Slot == EInventorySlot::Weapon2)
	{
		WeaponSlot2=WeaponToAssign;
	}
}


void APlayerCharacter::ClearWeaponSlot(EInventorySlot WeaponSlotIndex)
{
	if(WeaponSlotIndex==EInventorySlot::Weapon1)
		WeaponSlot1=nullptr;
	else if(WeaponSlotIndex==EInventorySlot::Weapon2)
		WeaponSlot2=nullptr;
}


void APlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	if(PrevMovementMode == MOVE_Falling && MoveComp->MovementMode != MOVE_Falling)
	{
		SetJumping(false);
	}
}


void APlayerCharacter::AddWeaponToInventory(AZWeapon* NewWeapon, EInventorySlot EmptySlot)
{
	if(!HasAuthority())
	{
		ServerAddWeaponToInventory(NewWeapon, EmptySlot);
	}
	else
	{
		if(EmptySlot == EInventorySlot::INVALID_SLOT)
		{
			if(ActiveWeapon->IsReloading())
			{
				ActiveWeapon->InterruptReload();
			}
			
			const EInventorySlot NewSlot = ActiveWeapon->GetInventorySlot();
			ActiveWeapon->WeaponDropped();
			ClearWeaponSlot(NewSlot);
			
			AssignWeaponToSlot(NewWeapon, NewSlot);
			NewWeapon->SetEquipped(true);
			NewWeapon->WeaponPickedUp(this, NewSlot);

			LastWeaponInHand = nullptr;
			ActiveWeapon = NewWeapon;
		}

		else
		{
			AssignWeaponToSlot(NewWeapon, EmptySlot);
			NewWeapon->SetEquipped(false);
			NewWeapon->WeaponPickedUp(this, EmptySlot);

			if(bAutoSwitchNewWeapon)
			{
				LastWeaponInHand = ActiveWeapon;
				ActiveWeapon = NewWeapon;
				ActiveWeapon->PullOutWeapon();
			}
		}
	}
}


void APlayerCharacter::ServerAddWeaponToInventory_Implementation(AZWeapon* NewWeapon, EInventorySlot EmptySlot)
{
	AddWeaponToInventory(NewWeapon, EmptySlot);
}


bool APlayerCharacter::ServerAddWeaponToInventory_Validate(AZWeapon* NewWeapon, EInventorySlot EmptySlot)
{
	return true;
}


void APlayerCharacter::TryPickupWeapon(AZWeapon* NewWeapon)
{
	//UE_LOG(LogTemp, Warning, TEXT("Local, Player pickup weapon called"));
	const EInventorySlot EmptySlot = GetFirstEmptyWeaponSlot();
	if(EmptySlot == EInventorySlot::INVALID_SLOT)
	{
		if(ActiveWeapon->IsReloading())
		{
			ActiveWeapon->InterruptReload();
		}
	}

	AddWeaponToInventory(NewWeapon, EmptySlot);
		//UE_LOG(LogTemp, Warning, TEXT("Pickup Weapon: %s"),*NewWeapon->GetName());
}


void APlayerCharacter::SetActiveWeaponState(EWeaponState NewWeaponState)
{
	ActiveWeaponState = NewWeaponState;
	MoveComp->SetActiveWeaponState(NewWeaponState);
}


float APlayerCharacter::GetWeaponZoomFOV_Implementation() const
{
	if(ActiveWeapon)
		return ActiveWeapon->GetZoomFOV();
	else
		return 0.f;
}


void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacter, LookPitch, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bIsAiming, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bWantsToSprint, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bIsJumping, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bIdleJump, COND_SkipOwner);
	DOREPLIFETIME(APlayerCharacter, WeaponSlot1);
	DOREPLIFETIME(APlayerCharacter, WeaponSlot2);
	DOREPLIFETIME(APlayerCharacter, ActiveWeapon);
	DOREPLIFETIME(APlayerCharacter, LastWeaponInHand);
}
