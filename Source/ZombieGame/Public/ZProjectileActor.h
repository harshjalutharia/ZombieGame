// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZProjectileActor.generated.h"

class UProjectileMovementComponent;
class USoundCue;

UCLASS()
class ZOMBIEGAME_API AZProjectileActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

public:

	UPROPERTY(VisibleAnywhere, Category = "Projectile Info")
	AController* InstigatorController;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Info")
	float DetonationTime;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Info")
	USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Info")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Info")
	UParticleSystem* ProjectileTrail;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Damage")
	float ExplosionRadialForceStrength;

	UPROPERTY(VisibleAnywhere, Category = "Projectile Damage")
	float MaxDamageOfProjectile;

	UPROPERTY(VisibleAnywhere, Category = "Projectile Damage")
	float MinDamageOfProjectile;

	UPROPERTY(VisibleAnywhere, Category = "Projectile Damage")
	float InnerExplosionRadius;

	UPROPERTY(VisibleAnywhere, Category = "Projectile Damage")
	float OuterExplosionRadius;
	
	UPROPERTY(VisibleAnywhere, Category = "Projectile Damage")
	float DamageFalloffExponent;

	UPROPERTY(VisibleAnywhere, Category = "Projectile Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	// Sets default values for this actor's properties
	AZProjectileActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnExplosionTimerExpired();

};
