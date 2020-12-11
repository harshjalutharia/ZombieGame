// Fill out your copyright notice in the Description page of Project Settings.


#include "ZProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
AZProjectileActor::AZProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	SetRootComponent(MeshComp);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionProfileName("PhysicsBody");
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");

	ExplosionRadialForceStrength = 500.f;

	SetReplicates(true);
	SetReplicatingMovement(true);
}

// Called when the game starts or when spawned
void AZProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	if(ProjectileTrail)
	{
		UGameplayStatics::SpawnEmitterAttached(ProjectileTrail,MeshComp);
	}

	FTimerHandle ExplosionTimer;
	GetWorldTimerManager().SetTimer(ExplosionTimer,this,&AZProjectileActor::OnExplosionTimerExpired,DetonationTime,false);
}


void AZProjectileActor::OnExplosionTimerExpired()
{
	// FUTURE OPTIMIZATION: Perform line traces to check how much of actor is exposed to the projectile
	if(HasAuthority())
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		
		UGameplayStatics::ApplyRadialDamageWithFalloff(this,MaxDamageOfProjectile,MinDamageOfProjectile,
			GetActorLocation(),InnerExplosionRadius,OuterExplosionRadius,DamageFalloffExponent,DamageTypeClass,
			IgnoredActors,this,InstigatorController,ECC_Visibility);

		TArray<FHitResult> HitResults;

		const bool bIsHit = GetWorld()->SweepMultiByChannel(HitResults,GetActorLocation(),GetActorLocation(),FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeSphere(InnerExplosionRadius));

		if(bIsHit)
		{
			for(auto Hit:HitResults)
			{
				UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());
				if(StaticMeshComp)
				{
					StaticMeshComp->AddRadialImpulse(GetActorLocation(),InnerExplosionRadius,ExplosionRadialForceStrength,RIF_Constant,true);
				}
            }
		}
	}

	if(ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());
	}

	if(ExplosionSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetLifeSpan(3.f);
}

