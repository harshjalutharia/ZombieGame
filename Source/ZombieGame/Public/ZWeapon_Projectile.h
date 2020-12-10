// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZWeapon.h"
#include "ZWeapon_Projectile.generated.h"

class AZProjectileActor;
class USoundCue;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API AZWeapon_Projectile : public AZWeapon
{
	GENERATED_BODY()

protected:

	AZWeapon_Projectile();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	float MaxDamageOfProjectile;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	float MinDamageOfProjectile;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	float InnerExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	float OuterExplosionRadius;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	float DamageFalloffExponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	TSubclassOf<AZProjectileActor> ProjectileClass;

	virtual void FireWeapon() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireWeapon();
	void ServerFireWeapon_Implementation();
	bool ServerFireWeapon_Validate();
	
};
