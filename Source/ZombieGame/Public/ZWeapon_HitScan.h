// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZWeapon.h"
#include "ZWeapon_HitScan.generated.h"

USTRUCT()
struct FHitScanImpact
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantize ImpactLocation;

	UPROPERTY()
	uint8 BurstCounter;
};

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API AZWeapon_HitScan : public AZWeapon
{
	GENERATED_BODY()

public:

	AZWeapon_HitScan();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	float HeadShotDamageMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	float BodyShotDamageMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	float ArmShotDamageMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	float LegShotDamageMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float WeaponRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float AllowedViewDotHitDir;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float ClientSideHitLeeway;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	uint8 TracerRoundInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float MinimumDistanceToSpawnTracer;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float TracerTravelDuration;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	class UNiagaraSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	FName TracerEndVectorParameterName;

	UPROPERTY(EditDefaultsOnly, Category ="Weapon Info")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category ="Weapon Info")
	UParticleSystem* BloodImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info", meta=(EditCondition="bAutomaticFire"))
	UCurveFloat* VerticalRecoilCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info", meta=(EditCondition="bAutomaticFire"))
	UCurveFloat* HorizontalRecoilCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float BulletRandomVerticalDeviationRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float BulletRandomHorizontalDeviationRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float MaxRecoilRecoveryTime;
	
	uint8 ShotCount;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanImpactNotify)
	FHitScanImpact HitScanImpactNotify;
	
	virtual void FireWeapon() override;
	
	void ProcessInstantHit(FHitResult& HitResult, FVector ShotDirection);

	void InstantHitConfirmed(FHitResult& HitResult, FVector ShotDirection);

	UFUNCTION()
	void OnRep_HitScanImpactNotify();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNotifyHit(FHitResult HitResult, FVector_NetQuantizeNormal ShotDirection);
	void ServerNotifyHit_Implementation(FHitResult HitResult, FVector_NetQuantizeNormal ShotDirection);
	bool ServerNotifyHit_Validate(FHitResult HitResult, FVector_NetQuantizeNormal ShotDirection);

	UFUNCTION(Server, Reliable)
    void ServerNotifyMiss(FVector_NetQuantizeNormal ShotDirection);
	void ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShotDirection);

	bool PerformWeaponTrace(FHitResult& HitResult, FVector TraceStartLocation, FVector TraceEndLocation) const;
	bool HitDirectionAcceptable(FVector& ShotEndLocation) const;

	void SimulateWeaponFire(FVector ImpactLocation);

	void PlayImpactEffects(FVector ImpactLocation, EPhysicalSurface ImpactSurface) const;
	void PlayTrailEffects(FVector ImpactLocation);

	void DoDamage(FHitResult& HitResult, FVector& ShotDirection);

	float LowestControlRotationPitch;

	float VerticalRecoilAmount;
	float HorizontalRecoilAmount;

	float OldVerticalRecoilAmount;
	float OldHorizontalRecoilAmount;

	float MaxVerticalRecoilReached;
	float MaxHorizontalRecoilReached;

	float RecoilRecoveryTime;
	float RecoilCalculationTime;

	float MaxRecoilCalculationTimeReached;

	void Recoil();

public:

	virtual void Tick(float DeltaSeconds) override;
	
};


