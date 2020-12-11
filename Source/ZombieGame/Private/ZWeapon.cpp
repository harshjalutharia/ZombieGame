// Fill out your copyright notice in the Description page of Project Settings.


#include "ZWeapon.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "ZPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"


#define DROPPED_WEAPON_COLLISION "DroppedWeapon"


// Sets default values
AZWeapon::AZWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComp");
	SetRootComponent(MeshComp);
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	BoxComp->SetupAttachment(RootComponent);

	ReloadTimeWithoutAnimation = 2.f;
	TimeBetweenShots = 0.5f;
	LastFireTime = 0.f;
	
	MeshComp->SetCollisionProfileName(DROPPED_WEAPON_COLLISION);
	MeshComp->SetSimulatePhysics(true);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AZWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentClipAmmo = MaxClipAmmo;
	CurrentReserveAmmo = MaxReserveAmmo;
	SetWeaponState(EWeaponState::Idle);
}


void AZWeapon::PlayFiringEffects() const
{
	if(!OwnerPlayer || (HasAuthority() && GetNetMode()==NM_DedicatedServer))
		return;
	
	if(OwnerPlayer->IsAiming() && FireAnim_Aiming)
	{
		OwnerPlayer->PlayAnimMontage(FireAnim_Aiming);
	}
	else if(FireAnim_Hip)
	{
		OwnerPlayer->PlayAnimMontage(FireAnim_Hip);
	}
	
	if(MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect,MeshComp,MuzzleSocketName);
	}

	if(FiringSound)
	{
		if(OwnerPlayer->IsLocallyControlled())
		{
			UGameplayStatics::SpawnSoundAttached(FiringSound,MeshComp);
			//UGameplayStatics::SpawnSound2D(this, FiringSound);
		}
		else
		{
			UGameplayStatics::SpawnSoundAtLocation(this, FiringSound,GetActorLocation());
		}
	}

	if(WeaponMesh_FireAnim)
	{
		MeshComp->PlayAnimation(WeaponMesh_FireAnim,false);
	}
	
	if(OwnerPlayer->IsLocallyControlled() && FireCameraShake)
	{
		AZPlayerController* PC = Cast<AZPlayerController>(OwnerPlayer->GetController());
		if(PC)
		{
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}


void AZWeapon::ServerFiringEffects_Implementation()
{
	if(GetNetMode()!=NM_DedicatedServer)
		PlayFiringEffects();
	
	MultiFiringEffects();
}


void AZWeapon::MultiFiringEffects_Implementation()
{
	if(OwnerPlayer!=nullptr && !OwnerPlayer->IsLocallyControlled())
	{
		PlayFiringEffects();
	}
}


void AZWeapon::HandleFiring()
{
	if(OwnerPlayer && OwnerPlayer->CanFire())
	{
		if(CanFire())
		{
			if(WeaponState != EWeaponState::Firing)
			{
				SetWeaponState(EWeaponState::Firing);
				ServerToggleFiringWeaponState(true);
			}

			UseAmmo();
			LastFireTime = GetWorld()->GetTimeSeconds();
			
			FireWeapon();
			ServerFiringEffects();
			
			if(!HasAuthority())
			{
				PlayFiringEffects();
			}
		}
	}
}


void AZWeapon::StartFiring()
{
	bWantsToFire = true;
	const float FirstShotDelay = FMath::Max(0.f,TimeBetweenShots - (GetWorld()->GetTimeSeconds() - LastFireTime));

	if(bAutomaticFire)
		GetWorldTimerManager().SetTimer(FiringTimerHandle,this, &AZWeapon::HandleFiring,TimeBetweenShots,true,FirstShotDelay);

	else if(FirstShotDelay==0)
		HandleFiring();
}


void AZWeapon::StopFiring()
{
	bWantsToFire = false;
	GetWorldTimerManager().ClearTimer(FiringTimerHandle);

	if(WeaponState==EWeaponState::Firing)
	{
		SetWeaponState(EWeaponState::Idle);
		ServerToggleFiringWeaponState(false);
	}

	if(CurrentClipAmmo==0 && CurrentReserveAmmo > 0)
	{
		ReloadWeapon();
	}
}


void AZWeapon::ReloadWeapon()
{
	if(!CanReload())
		return;

	SetWeaponState(EWeaponState::Reloading);

	const float Duration = PlayReloadEffects();

	GetWorldTimerManager().SetTimer(ReloadTimerHandle,this,&AZWeapon::ReloadComplete,Duration,false,Duration);

	if(!HasAuthority())
	{
		ServerReloadWeapon(CurrentClipAmmo, CurrentReserveAmmo);
	}
	else
	{
		MultiPlayReloadEffects();
	}
}


void AZWeapon::ServerReloadWeapon_Implementation(int32 ClientClipAmmo, int32 ClientReserveAmmo)
{
	ReloadWeapon();
}


bool AZWeapon::ServerReloadWeapon_Validate(int32 ClientClipAmmo, int32 ClientReserveAmmo)
{
	if(ClientReserveAmmo==0 || ClientClipAmmo>CurrentClipAmmo || ClientReserveAmmo>CurrentReserveAmmo)
		return false;
	return true;
}


void AZWeapon::ReloadComplete()
{
	SetWeaponState(EWeaponState::Idle);

	const int32 BulletsUsed = MaxClipAmmo-CurrentClipAmmo;

	if (BulletsUsed > CurrentReserveAmmo)
	{
		CurrentClipAmmo+=CurrentReserveAmmo;
		CurrentReserveAmmo = 0;
	}
	else
	{
		CurrentClipAmmo=MaxClipAmmo;
		CurrentReserveAmmo-=BulletsUsed;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Local Ammo: %s / %s"),*FString::FromInt(CurrentClipAmmo),*FString::FromInt(CurrentReserveAmmo));
}


float AZWeapon::PlayReloadEffects()
{
	if(!OwnerPlayer)
		return 0.f;

	float Duration = ReloadTimeWithoutAnimation;
	
	if(ReloadAnim)
	{
		if(HasAuthority() && GetNetMode()==NM_DedicatedServer)
		{
			Duration = ReloadAnim->GetPlayLength();
		}
		else
		{
			Duration = OwnerPlayer->PlayAnimMontage(ReloadAnim);
		}
	}

	if(ReloadSound)
	{
		if(OwnerPlayer->IsLocallyControlled())
		{
			ReloadSoundBeingPlayed = UGameplayStatics::SpawnSound2D(this, ReloadSound);
		}
		else
		{
			ReloadSoundBeingPlayed = UGameplayStatics::SpawnSoundAttached(ReloadSound,GetRootComponent());
		}
	}

	if(WeaponMesh_ReloadAnim)
	{
		MeshComp->PlayAnimation(WeaponMesh_ReloadAnim, false);
	}

	return Duration;
}


void AZWeapon::StopReloadEffects() const
{
	if(!OwnerPlayer)
		return;

	if(ReloadAnim && !(HasAuthority() && GetNetMode()==NM_DedicatedServer))
	{
		OwnerPlayer->StopAnimMontage(ReloadAnim);		
	}

	if(ReloadSound && ReloadSoundBeingPlayed!=nullptr)
	{
		if(ReloadSoundBeingPlayed->IsPlaying())
		{
			ReloadSoundBeingPlayed->Stop();
		}
	}

	if(WeaponMesh_ReloadAnim)
	{
		MeshComp->Stop();
	}
}


void AZWeapon::MultiPlayReloadEffects_Implementation()
{
	if(OwnerPlayer && OwnerPlayer->GetLocalRole()==ROLE_SimulatedProxy)
	{
		PlayReloadEffects();
	}
}

void AZWeapon::MultiStopReloadEffects_Implementation()
{
	if(OwnerPlayer && OwnerPlayer->GetLocalRole()==ROLE_SimulatedProxy)
	{
		StopReloadEffects();
	}
}


void AZWeapon::UseAmmo()
{
	if(!HasAuthority())
	{
		ServerUseAmmo(CurrentClipAmmo);
	}
	
	CurrentClipAmmo--;
}


void AZWeapon::ServerUseAmmo_Implementation(int32 ClientClipAmmo)
{
	UseAmmo();
}


bool AZWeapon::ServerUseAmmo_Validate(int32 ClientClipAmmo)
{
	if(ClientClipAmmo < 0 || ClientClipAmmo>MaxClipAmmo)
		return false;
	return true;
}


void AZWeapon::InterruptReload()
{
	if(WeaponState != EWeaponState::Reloading)
		return;

	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

	SetWeaponState(EWeaponState::Idle);

	if(!HasAuthority() || GetNetMode()!=NM_DedicatedServer)
	{
		StopReloadEffects();
	}
	
	if(HasAuthority())
	{
		MultiStopReloadEffects();
	}
}


void AZWeapon::WeaponPickedUp(APlayerCharacter* PC, EInventorySlot SlotAssigned)
{
	if(!PC)
		return;

	if(HasAuthority())
	{
		OwnerPlayer = PC;
		SlotAssignedToWeapon = SlotAssigned;
		SetInstigator(OwnerPlayer);
	}
	
	SetOwner(OwnerPlayer);
	
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if(bIsEquipped)
	{
		AttachWeaponToAssignedSlotOrHand(true);
		
		if(OwnerPlayer->IsLocallyControlled())
			if(CurrentClipAmmo==0 && CurrentReserveAmmo > 0)
				ReloadWeapon();
	}
	else
		AttachWeaponToAssignedSlotOrHand(false);
	//EquipComplete();
	//const float Duration = PlayPickupAnimation();
	//GetWorldTimerManager().SetTimer(WeaponEquippingTimer,this, &AZWeapon::EquipComplete, Duration, false, Duration);
}


void AZWeapon::WeaponDropped()
{
	if(HasAuthority())
	{
		OwnerPlayer = nullptr;
		SlotAssignedToWeapon = EInventorySlot::INVALID_SLOT;
		SetInstigator(nullptr);
		bIsEquipped = false;
	}

	SetOwner(nullptr);

	const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld,false);
	this->DetachFromActor(DetachmentTransformRules);
	
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetSimulatePhysics(true);
}


void AZWeapon::PullOutWeapon()
{
	AZWeapon* WeaponInHand = OwnerPlayer->GetLastWeaponInHand();

	if(WeaponInHand->IsReloading())
	{
		WeaponInHand->InterruptReload();
	}
	
	float Duration = PlayWeaponSwitchAnimation();

	if(OwnerPlayer->IsLocallyControlled() || HasAuthority())
	{
		SetWeaponState(EWeaponState::Equipping);
	
		GetWorldTimerManager().SetTimer(WeaponEquippingTimer, this, &AZWeapon::EquipComplete,Duration,false,Duration);
	}
}


void AZWeapon::SwapWeaponsDuringAnimation()
{
	AZWeapon* WeaponInHand = OwnerPlayer->GetLastWeaponInHand();

	if(!WeaponInHand)
	{
		UE_LOG(LogTemp, Warning, TEXT("LastWeapon in PlayerCharacter not found by %s weapon"), *GetName())
		return;
	}

	const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld,false);
	
	WeaponInHand->DetachFromActor(DetachmentTransformRules);
	this->DetachFromActor(DetachmentTransformRules);

	WeaponInHand->AttachWeaponToAssignedSlotOrHand(false);
	this->AttachWeaponToAssignedSlotOrHand(true);
}


void AZWeapon::EquipComplete()
{
	SetWeaponState(EWeaponState::Idle);

	if(CurrentClipAmmo==0 && CurrentReserveAmmo > 0)
	{
		if(OwnerPlayer && OwnerPlayer->IsLocallyControlled())
		{
			ReloadWeapon();
		}
	}
}


void AZWeapon::AttachWeaponToAssignedSlotOrHand(bool bAttachToHand)
{
	if(OwnerPlayer)
	{
		if(bAttachToHand)
			AttachToComponent(OwnerPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, OwnerPlayer->GetHandAttachPoint());
		else
			AttachToComponent(OwnerPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, OwnerPlayer->GetAttachPointOfSlot(SlotAssignedToWeapon));
	}
}


float AZWeapon::PlayWeaponSwitchAnimation() const
{
	if(!OwnerPlayer)
		return 0.f;

	float Duration = WeaponSwitchTimeWithoutAnimation;
	
	if(WeaponSwitchAnim)
	{
		if(GetNetMode()!=NM_DedicatedServer || !HasAuthority())
		{
			Duration = OwnerPlayer->PlayAnimMontage(WeaponSwitchAnim);
		}
		else
		{
			Duration = WeaponSwitchAnim->GetPlayLength();
		}
	}

	return Duration;
}


void AZWeapon::StopWeaponSwitchAnimation() const
{
	if(!OwnerPlayer)
		return;

	if(WeaponSwitchAnim && !(HasAuthority() && GetNetMode()==NM_DedicatedServer))
	{
		OwnerPlayer->StopAnimMontage(WeaponSwitchAnim);
	}
}


void AZWeapon::MultiPlayWeaponSwitchAnimation_Implementation()
{
	if(OwnerPlayer && OwnerPlayer->GetLocalRole()==ROLE_SimulatedProxy)
	{
		PlayWeaponSwitchAnimation();
	}
}


void AZWeapon::MultiStopWeaponSwitchAnimation_Implementation()
{
	if(OwnerPlayer && OwnerPlayer->GetLocalRole()==ROLE_SimulatedProxy)
	{
		StopWeaponSwitchAnimation();
	}
}


void AZWeapon::OnRep_OwnerPlayer()
{
	if(OwnerPlayer!=nullptr)
	{
		WeaponPickedUp(OwnerPlayer, SlotAssignedToWeapon);
	}
	else
	{
		WeaponDropped();
	}
}


void AZWeapon::SetWeaponState(EWeaponState NewWeaponState)
{
	WeaponState = NewWeaponState;
	if(OwnerPlayer)
	{
		OwnerPlayer->SetActiveWeaponState(NewWeaponState);
	}
}


void AZWeapon::ServerToggleFiringWeaponState_Implementation(bool IsFiring)
{
	if(IsFiring)
		WeaponState = EWeaponState::Firing;
	else
		WeaponState = EWeaponState::Idle;
}


bool AZWeapon::ServerToggleFiringWeaponState_Validate(bool IsFiring)
{
	return true;
}


void AZWeapon::OnRep_WeaponState()
{
	if(OwnerPlayer)
		OwnerPlayer->SetActiveWeaponState(WeaponState);
}


/*void AZWeapon::OnRep_SlotAssignedToWeapon()
{
	if(OwnerPlayer->GetActiveWeaponIndex()==EInventorySlot::Hands)
	{
		AttachToComponent(OwnerPlayer->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,OwnerPlayer->GetAttachPointOfSlot(EInventorySlot::Hands));
		OwnerPlayer->SetActiveWeaponIndex(SlotAssignedToWeapon);
	}
	else
	{
		AttachToComponent(OwnerPlayer->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,OwnerPlayer->GetAttachPointOfSlot(SlotAssignedToWeapon));
	}
}*/


void AZWeapon::Interact(APlayerCharacter* PlayerInteracted)
{
	//UE_LOG(LogTemp, Warning, TEXT("Local, Weapon interact called"));
	if(OwnerPlayer)
		return;
	
	if(PlayerInteracted)
	{
		PlayerInteracted->TryPickupWeapon(this);
	}
}


UTexture* AZWeapon::GetCustomCrosshairIfApplicable() const
{
	if(bUseCustomCrosshair && CrosshairImage)
		return CrosshairImage;
	
	return nullptr;
}


void AZWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZWeapon, OwnerPlayer);
	DOREPLIFETIME(AZWeapon, SlotAssignedToWeapon);
	DOREPLIFETIME(AZWeapon, bIsEquipped);
	DOREPLIFETIME_CONDITION(AZWeapon, WeaponState, COND_SkipOwner);
}
