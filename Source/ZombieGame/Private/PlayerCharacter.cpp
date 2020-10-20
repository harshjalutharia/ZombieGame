// Fill out your copyright notice in the Description page of Project Settings.

#include "ZombieGame/Public/PlayerCharacter.h"
#include "ZombieGame/Public/PlayerCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ZWeapon.h"
#include "ZHealthComponent.h"
#include "Net/UnrealNetwork.h"

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
	NumberOfWeaponSlots = 3;
	ActiveWeaponIndex = NO_ACTIVE_WEAPON;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		if(!bSpawnWithWeapon || !SpawnWithWeaponClass)
			return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AZWeapon* NewWeapon = GetWorld()->SpawnActor<AZWeapon>(SpawnWithWeaponClass,FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);
		if(NewWeapon)
		{
			WeaponsOnPlayer.Add(NewWeapon);
			ActiveWeaponIndex=0;
			
			NewWeapon->WeaponEquipped(this);
			NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ActiveWeaponSocketName);
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsLocallyControlled() || HasAuthority())
		SetLookPitch();
	
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

	PlayerInputComponent->BindAction("DropWeapon",IE_Pressed, this, &APlayerCharacter::DropWeapon);

	PlayerInputComponent->BindAction("LastWeapon",IE_Pressed, this, &APlayerCharacter::QuickSwitchWeapon);

	PlayerInputComponent->BindAction("WeaponSlot1",IE_Pressed, this, &APlayerCharacter::UseWeaponSlot1);
	PlayerInputComponent->BindAction("WeaponSlot2",IE_Pressed, this, &APlayerCharacter::UseWeaponSlot2);
	PlayerInputComponent->BindAction("WeaponSlot3",IE_Pressed, this, &APlayerCharacter::UseWeaponSlot3);

	PlayerInputComponent->BindAction("Interact",IE_Pressed, this, &APlayerCharacter::Interact);
}


bool APlayerCharacter::IsSprinting() const
{
	// Ensure that changes made here are also made in Tick function of UPlayerCharacterMovementComponent
	return (!bIsAiming && !bIsCrouched && !bIsReloading && bWantsToSprint && 
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
	SetAiming(true);
}


void APlayerCharacter::StopAiming()
{
	SetAiming(false);
}


void APlayerCharacter::RequestJump()
{
	SetJumping(true);
}


void APlayerCharacter::StartFiring()
{
	if(WeaponsOnPlayer[ActiveWeaponIndex])
	{
		WeaponsOnPlayer[ActiveWeaponIndex]->StartFiring();
	}
}


void APlayerCharacter::StopFiring()
{
	if(WeaponsOnPlayer[ActiveWeaponIndex])
	{
		WeaponsOnPlayer[ActiveWeaponIndex]->StopFiring();
	}
}


void APlayerCharacter::RequestReload()
{
	if(WeaponsOnPlayer[ActiveWeaponIndex])
	{
		WeaponsOnPlayer[ActiveWeaponIndex]->ReloadWeapon();
	}
}


void APlayerCharacter::DropWeapon()
{
	if(ActiveWeaponIndex != NO_ACTIVE_WEAPON)
	{
		if(!HasAuthority())
		{
			ServerDropWeapon();
		}
		
		AZWeapon* WeaponInHand = WeaponsOnPlayer[ActiveWeaponIndex];
		if(WeaponInHand->IsReloading())
		{
			WeaponInHand->InterruptReload();
		}

		if(HasAuthority())
		{
			const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld,false);
			
			WeaponInHand->DetachFromActor(DetachmentTransformRules);
			WeaponInHand->WeaponDropped();
		}
	}
}


void APlayerCharacter::QuickSwitchWeapon()
{
}


void APlayerCharacter::UseWeaponSlot1()
{
	
}


void APlayerCharacter::UseWeaponSlot2()
{
}


void APlayerCharacter::UseWeaponSlot3()
{
}


void APlayerCharacter::Interact()
{
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


void APlayerCharacter::ServerDropWeapon_Implementation()
{
	DropWeapon();
}


bool APlayerCharacter::ServerDropWeapon_Validate()
{
	if(ActiveWeaponIndex == NO_ACTIVE_WEAPON)
		return false;

	return true;
}


void APlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	if(PrevMovementMode == MOVE_Falling && MoveComp->MovementMode != MOVE_Falling)
	{
		SetJumping(false);
	}
}


void APlayerCharacter::SetReloading(bool Reloading)
{
	bIsReloading = Reloading;
	MoveComp->SetReloading(Reloading);
}


void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacter, LookPitch, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bIsAiming, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bWantsToSprint, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bIsJumping, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bIdleJump, COND_SkipOwner);
	DOREPLIFETIME(APlayerCharacter, ActiveWeaponIndex);
	DOREPLIFETIME(APlayerCharacter, WeaponsOnPlayer);
}
