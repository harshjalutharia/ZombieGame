// Fill out your copyright notice in the Description page of Project Settings.


#include "ZWeapon.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerCharacter.h"
#include "ZPlayerController.h"
#include "Kismet/GameplayStatics.h"


#define DROPPED_WEAPON_COLLISION "DroppedWeapon"


// Sets default values
AZWeapon::AZWeapon()
{
	//RootComp = CreateDefaultSubobject<USceneComponent>("SceneComp");
	//SetRootComponent(RootComp);

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComp");
	SetRootComponent(MeshComp);
	//MeshComp->SetupAttachment(RootComponent);
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	BoxComp->SetupAttachment(RootComponent);
	//SetRootComponent(BoxComp);

	ReloadTimeWithoutAnimation = 2.f;
	TimeBetweenShots = 0.5f;
	LastFireTime = 0.f;
	
	MeshComp->SetCollisionProfileName(DROPPED_WEAPON_COLLISION);
	//MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	MeshComp->SetSimulatePhysics(true);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AZWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentClipAmmo = MaxClipAmmo;
	CurrentReserveAmmo = MaxReserveAmmo;
}


void AZWeapon::PlayFiringEffects() const
{
	if(!OwnerPlayer)
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
	if(OwnerPlayer && !OwnerPlayer->IsJumping() && !OwnerPlayer->IsSprinting())
	{
		if(!bIsReloading && CurrentClipAmmo>0)
		{
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

	if(CurrentClipAmmo==0 && CurrentReserveAmmo > 0)
	{
		ReloadWeapon();
	}
}


void AZWeapon::ReloadWeapon()
{
	if(bIsReloading || CurrentClipAmmo==MaxClipAmmo || CurrentReserveAmmo==0 || bWantsToFire || !OwnerPlayer)
		return;

	bIsReloading = true;
	OwnerPlayer->SetReloading(true);

	const float Duration = PlayReloadAnimation();

	GetWorldTimerManager().SetTimer(ReloadTimerHandle,this,&AZWeapon::ReloadComplete,Duration,false,Duration);

	if(!HasAuthority())
	{
		ServerReloadWeapon(CurrentClipAmmo,CurrentReserveAmmo);
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
	if(CurrentClipAmmo != ClientClipAmmo || CurrentReserveAmmo != ClientReserveAmmo)
		return false;
	return true;
}


void AZWeapon::ReloadComplete()
{
	bIsReloading = false;
	OwnerPlayer->SetReloading(false);

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

	UE_LOG(LogTemp, Warning, TEXT("Local Ammo: %s / %s"),*FString::FromInt(CurrentClipAmmo),*FString::FromInt(CurrentReserveAmmo));
}


float AZWeapon::PlayReloadAnimation() const
{
	if(!OwnerPlayer)
		return 0.f;

	float Duration = ReloadTimeWithoutAnimation;
	
	if(ReloadAnim)
	{
		Duration = OwnerPlayer->PlayAnimMontage(ReloadAnim);
	}

	return Duration;
}

void AZWeapon::StopReloadAnimation() const
{
	if(!OwnerPlayer)
		return;

	if(ReloadAnim)
	{
		OwnerPlayer->StopAnimMontage(ReloadAnim);		
	}
}


void AZWeapon::MultiPlayReloadEffects_Implementation()
{
	if(OwnerPlayer && OwnerPlayer->GetLocalRole()==ROLE_SimulatedProxy)
	{
		PlayReloadAnimation();
	}
}

void AZWeapon::MultiStopReloadEffects_Implementation()
{
	if(OwnerPlayer && OwnerPlayer->GetLocalRole()==ROLE_SimulatedProxy)
	{
		StopReloadAnimation();
	}
}


void AZWeapon::UseAmmo()
{
	CurrentClipAmmo--;
	
	if(!HasAuthority())
	{
		ServerUseAmmo(CurrentClipAmmo);
	}
}


void AZWeapon::ServerUseAmmo_Implementation(int32 ClientClipAmmo)
{
	UseAmmo();
}


bool AZWeapon::ServerUseAmmo_Validate(int32 ClientClipAmmo)
{
	if(ClientClipAmmo < 0 || CurrentClipAmmo<=0)
		return false;
	if(ClientClipAmmo != CurrentClipAmmo-1)
		return false;
	if(bIsReloading)
		return false;
	return true;
}


void AZWeapon::MultiEquipWeapon_Implementation()
{
	WeaponEquipped(nullptr);
}


void AZWeapon::MultiDropWeapon_Implementation()
{
	WeaponDropped();
}


void AZWeapon::InterruptReload()
{
	if(!bIsReloading)
		return;

	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

	bIsReloading = false;
	OwnerPlayer->SetReloading(false);

	if(GetNetMode()!=NM_DedicatedServer)
	{
		StopReloadAnimation();
	}
	
	if(HasAuthority())
	{
		MultiStopReloadEffects();
	}
}


void AZWeapon::WeaponEquipped(APlayerCharacter* PC)
{
	if(HasAuthority())
	{
		SetInstigator(PC);
		OwnerPlayer = PC;
		SetReplicateMovement(false);
		bIsEquipped = true;
	}

	//MeshComp->SetCollisionProfileName("NoCollision");
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AZWeapon::WeaponDropped()
{
	if(HasAuthority())
	{
		SetInstigator(nullptr);
		OwnerPlayer = nullptr;
		SetReplicateMovement(true);
		bIsEquipped = false;
	}

	//MeshComp->SetCollisionProfileName(DROPPED_WEAPON_COLLISION);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	MeshComp->SetSimulatePhysics(true);
}


void AZWeapon::OnRep_IsEquipped()
{
	if(bIsEquipped)
	{
		WeaponEquipped(nullptr);
	}
	else
	{
		WeaponDropped();
	}
}


void AZWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AZWeapon, bIsReloading, COND_OwnerOnly);
	DOREPLIFETIME(AZWeapon, OwnerPlayer);
	DOREPLIFETIME(AZWeapon, bIsEquipped);
	
}
