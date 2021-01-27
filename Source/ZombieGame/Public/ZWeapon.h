// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ZINT_InteractableActor.h"
#include "ZombieGame/ZEnums.h"
#include "ZWeapon.generated.h"


UCLASS()
class ZOMBIEGAME_API AZWeapon : public AActor, public IZINT_InteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_OwnerPlayer)
	class APlayerCharacter* OwnerPlayer;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_WeaponState)
	EWeaponState WeaponState;

	UPROPERTY(Replicated)//Using=OnRep_SlotAssignedToWeapon)
	EInventorySlot SlotAssignedToWeapon;

	/*
	WEAPON CONFIGURATION
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float DamagePerShot;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	int32 MaxReserveAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	int32 MaxClipAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	bool bAutomaticFire;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float ReloadTimeWithoutAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float WeaponSwitchTimeWithoutAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float ZoomFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	UAnimMontage* FireAnim_Hip;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	UAnimMontage* FireAnim_Aiming;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	UAnimMontage* ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	UAnimMontage* WeaponSwitchAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	UAnimationAsset* WeaponMesh_FireAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	UAnimationAsset* WeaponMesh_ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category ="Weapon Info")
	UParticleSystem* MuzzleEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, Category ="Weapon Info")
	USoundBase* FiringSound;
	
	UPROPERTY(EditDefaultsOnly, Category ="Weapon Info")
	USoundBase* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category ="Weapon Info")
	UTexture2D* WeaponImageUI;

	/*UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	bool bUseCustomCrosshair;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info", meta = (EditCondition = "bUseCustomCrosshair"))
	UTexture* CrosshairImage;*/

	/*
	COMPONENTS
	*/

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USkeletalMeshComponent* MeshComp;

	/*
	FIRING STUFF
	*/

	bool CanFire() const { return (OwnerPlayer && (WeaponState==EWeaponState::Idle || WeaponState==EWeaponState::Firing) && CurrentClipAmmo>0); }
	bool CanReload() const { return (OwnerPlayer && WeaponState==EWeaponState::Idle && CurrentClipAmmo!=MaxClipAmmo && CurrentReserveAmmo!=0); }

	bool bWantsToFire;
	
	FTimerHandle FiringTimerHandle;

	float LastFireTime;

	void PlayFiringEffects() const;

	UFUNCTION(Server, Reliable)
	void ServerFiringEffects();
	void ServerFiringEffects_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void MultiFiringEffects();
	void MultiFiringEffects_Implementation();

	void HandleFiring();

	virtual void FireWeapon() PURE_VIRTUAL(AZWeapon::FireWeapon, );

public:
		
	void StartFiring();

	void StopFiring();

	FVector GetMuzzleLocation() const { return MeshComp->GetSocketLocation(MuzzleSocketName); }
	FRotator GetMuzzleRotation() const { return MeshComp->GetSocketRotation(MuzzleSocketName); }

	/*
	RELOADING STUFF
	*/

	void ReloadWeapon();

protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReloadWeapon(int32 ClientClipAmmo, int32 ClientReserveAmmo);
	void ServerReloadWeapon_Implementation(int32 ClientClipAmmo, int32 ClientReserveAmmo);
	bool ServerReloadWeapon_Validate(int32 ClientClipAmmo, int32 ClientReserveAmmo);

	void ReloadComplete();

	float PlayReloadEffects();
	void StopReloadEffects() const;

	UFUNCTION(NetMulticast, Reliable)
    void MultiPlayReloadEffects();
	void MultiPlayReloadEffects_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void MultiStopReloadEffects();
	void MultiStopReloadEffects_Implementation();

	UPROPERTY()
	UAudioComponent* ReloadSoundBeingPlayed;
	
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Info")
	int32 CurrentReserveAmmo;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Info")
	int32 CurrentClipAmmo;

	void UseAmmo();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseAmmo(int32 ClientClipAmmo);
	void ServerUseAmmo_Implementation(int32 ClientClipAmmo);
	bool ServerUseAmmo_Validate(int32 ClientClipAmmo);

public:

	bool IsReloading() const { return WeaponState==EWeaponState::Reloading; }

	void InterruptReload();

	int32 GetCurrentClipAmmo() const { return CurrentClipAmmo; }
	int32 GetCurrentReserveAmmo() const { return CurrentReserveAmmo; }
	int32 GetMaxClipAmmo() const { return MaxClipAmmo; }
	int32 GetMaxReserveAmmo() const { return MaxReserveAmmo; }

	/*
	PICKUP AND DROP
	*/

	EInventorySlot GetInventorySlot() const { return SlotAssignedToWeapon; }

	bool IsEquipping() const { return WeaponState==EWeaponState::Equipping; }

	void WeaponPickedUp(class APlayerCharacter* PC, EInventorySlot SlotAssigned);
	void WeaponDropped();

	void PullOutWeapon();

	UFUNCTION(BlueprintCallable)
	void SwapWeaponsDuringAnimation();

	void AttachWeaponToAssignedSlotOrHand(bool bAttachToHand);

	virtual void Interact_Implementation(APlayerCharacter* PlayerInteracted) override;

	void SetEquipped(bool bNewEquipped) { bIsEquipped = bNewEquipped; }

protected:

	UPROPERTY(Replicated)
	bool bIsEquipped;

	UFUNCTION()
	void EquipComplete();

	float PlayWeaponSwitchAnimation() const;
	void StopWeaponSwitchAnimation() const;

	UFUNCTION(NetMulticast, Reliable)
    void MultiPlayWeaponSwitchAnimation();
	void MultiPlayWeaponSwitchAnimation_Implementation();

	UFUNCTION(NetMulticast, Reliable)
    void MultiStopWeaponSwitchAnimation();
	void MultiStopWeaponSwitchAnimation_Implementation();

	FTimerHandle WeaponEquippingTimer;

	UFUNCTION()
	void OnRep_OwnerPlayer();

	/*UFUNCTION()
	void OnRep_SlotAssignedToWeapon();*/

	void SetWeaponState(EWeaponState NewWeaponState);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerToggleFiringWeaponState(bool IsFiring);
	void ServerToggleFiringWeaponState_Implementation(bool IsFiring);
	bool ServerToggleFiringWeaponState_Validate(bool IsFiring);

	UFUNCTION()
	void OnRep_WeaponState();

	/*
	MISCELLANEOUS STUFF
	*/

public:

	float GetZoomFOV() const { return ZoomFOV; }

	UTexture2D* GetWeaponImageUI() const { return WeaponImageUI; }

	/*UTexture* GetCustomCrosshairIfApplicable() const;*/
};
