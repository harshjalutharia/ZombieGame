// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZWeapon.generated.h"


UCLASS()
class ZOMBIEGAME_API AZWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	class APlayerCharacter* OwnerPlayer;

	/*
	WEAPON CONFIGURATION
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
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
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	UAnimMontage* FireAnim_Hip;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	UAnimMontage* FireAnim_Aiming;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	UAnimMontage* ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category ="Weapon VFX")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category ="Weapon VFX")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category ="Weapon VFX")
	UParticleSystem* BloodImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon VFX")
	TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipped)
	bool bIsEquipped;

	UFUNCTION()
	void OnRep_IsEquipped();

	/*
	COMPONENTS
	*/

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USkeletalMeshComponent* MeshComp;

	/*
	FIRING STUFF
	*/

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

	float PlayReloadAnimation() const;

	void StopReloadAnimation() const;

	UFUNCTION(NetMulticast, Reliable)
    void MultiPlayReloadEffects();
	void MultiPlayReloadEffects_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void MultiStopReloadEffects();
	void MultiStopReloadEffects_Implementation();
	
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Info")
	int32 CurrentReserveAmmo;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Info")
	int32 CurrentClipAmmo;

	UPROPERTY(Replicated)
	bool bIsReloading;

	void UseAmmo();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseAmmo(int32 ClientClipAmmo);
	void ServerUseAmmo_Implementation(int32 ClientClipAmmo);
	bool ServerUseAmmo_Validate(int32 ClientClipAmmo);

	/*
	 *MISC STUFF
	 */

	UFUNCTION(NetMulticast, Reliable)
    void MultiEquipWeapon();

	UFUNCTION(NetMulticast, Reliable)
	void MultiDropWeapon();

public:

	void InterruptReload();

	void WeaponEquipped(class APlayerCharacter* PC);
	void WeaponDropped();
	
	bool IsReloading() const { return bIsReloading; }

	int32 GetCurrentClipAmmo() const { return CurrentClipAmmo; }
	int32 GetCurrentReserveAmmo() const { return CurrentReserveAmmo; }
	int32 GetMaxClipAmmo() const { return MaxClipAmmo; }
	int32 GetMaxReserveAmmo() const { return MaxReserveAmmo; }
};
