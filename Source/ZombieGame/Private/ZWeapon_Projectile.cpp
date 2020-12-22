// Fill out your copyright notice in the Description page of Project Settings.


#include "ZWeapon_Projectile.h"
#include "PlayerCharacter.h"
#include "ZProjectileActor.h"
#include "Sound/SoundCue.h"


AZWeapon_Projectile::AZWeapon_Projectile()
{
	MaxDamageOfProjectile = 100.f;
	MinDamageOfProjectile = 10.f;
	InnerExplosionRadius = 300.f;
	OuterExplosionRadius = 600.f;
	DamageFalloffExponent = 1.f;
}


void AZWeapon_Projectile::FireWeapon()
{
	if(ProjectileClass==nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile class not set up in gun blueprint"))
		return;
	}

	if(!HasAuthority())
	{
		
		ServerFireWeapon();
	}

	else
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		OwnerPlayer->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
		
		const FVector MuzzleLocation = GetMuzzleLocation();
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = OwnerPlayer;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AZProjectileActor* Projectile = GetWorld()->SpawnActor<AZProjectileActor>(ProjectileClass, MuzzleLocation, CameraRotation, SpawnParams);
		Projectile->InstigatorController = OwnerPlayer->GetController();
		Projectile->DamageTypeClass = DamageTypeClass;
		Projectile->InnerExplosionRadius = InnerExplosionRadius;
		Projectile->OuterExplosionRadius = OuterExplosionRadius;
		Projectile->MaxDamageOfProjectile = MaxDamageOfProjectile;
		Projectile->MinDamageOfProjectile = MinDamageOfProjectile;
		Projectile->DamageFalloffExponent = DamageFalloffExponent;
	}
}


void AZWeapon_Projectile::ServerFireWeapon_Implementation()
{
	FireWeapon();
}


bool AZWeapon_Projectile::ServerFireWeapon_Validate()
{
	return true;
}
