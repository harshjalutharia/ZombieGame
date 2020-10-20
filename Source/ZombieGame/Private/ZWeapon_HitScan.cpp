// Fill out your copyright notice in the Description page of Project Settings.


#include "ZWeapon_HitScan.h"
#include "ZombieGame/ZombieGame.h"
#include "PlayerCharacter.h"
#include "ZPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


AZWeapon_HitScan::AZWeapon_HitScan()
{
	WeaponRange = 30000.f;
	AllowedViewDotHitDir = -1.f;
	ClientSideHitLeeway = 200.f;
	TracerRoundInterval = 3;
	MinimumDistanceToSpawnTracer = 600.f;

	HeadShotDamageMultiplier = 1.f;
	BodyShotDamageMultiplier = 1.f;
	ArmShotDamageMultiplier = 1.f;
	LegShotDamageMultiplier = 1.f;
}


void AZWeapon_HitScan::FireWeapon()
{
	AZPlayerController* PC = Cast<AZPlayerController>(OwnerPlayer->Controller);

	if(!PC)
		return;

	LastFireTime = GetWorld()->GetTimeSeconds();
	UseAmmo();

	FVector CameraLocation;
	FRotator CameraRotation;
	
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector CameraTraceStartLocation = CameraLocation + CameraRotation.Vector() * (FVector::DotProduct((GetActorLocation() - CameraLocation),CameraRotation.Vector()));
	const FVector CameraTraceEndLocation = CameraLocation + CameraRotation.Vector() * WeaponRange;

	FHitResult HitResult;
	const bool PreHit = PerformWeaponTrace(HitResult, CameraTraceStartLocation, CameraTraceEndLocation);
		
	if(PreHit)
	{
		const FVector BulletStartLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		const FVector BulletEndLocation = HitResult.Location + CameraRotation.Vector();
		PerformWeaponTrace(HitResult, BulletStartLocation, BulletEndLocation); 
	}
	else
	{
		HitResult.Location = FVector_NetQuantize(CameraTraceEndLocation);
	}

						//UE_LOG(LogTemp, Warning, TEXT("Local, Bullet: Hit %s"),*BulletHitResult.Actor->GetName());
						//DrawDebugLine(GetWorld(), BulletStartLocation, BulletHitResult.Location, FColor::Red, false,2.f,0.f,1.f);
						//UE_LOG(LogTemp, Warning, TEXT("Local, Bullet: No Hit"));
						//DrawDebugLine(GetWorld(), BulletStartLocation, BulletEndLocation, FColor::Red, false,2.f,0.f,1.f);
	ProcessInstantHit(HitResult, (HitResult.Location - MeshComp->GetSocketLocation(MuzzleSocketName)).GetSafeNormal());
}


void AZWeapon_HitScan::ProcessInstantHit(FHitResult& HitResult, FVector ShotDirection)
{
	// Call server functions if client
	if(OwnerPlayer && OwnerPlayer->GetNetMode()==NM_Client)
	{
		// if Server has authority over HitActor call server hit function
		if(HitResult.GetActor() && HitResult.GetActor()->GetRemoteRole()==ROLE_Authority)
		{
			ServerNotifyHit(HitResult,ShotDirection);
		}
		else if(HitResult.GetActor()==nullptr)
		{
			if(HitResult.bBlockingHit)
			{
				ServerNotifyHit(HitResult,ShotDirection);
			}
			else
			{
				ServerNotifyMiss(ShotDirection);
			}
		}
	}
	// On client: play vfx
	// On server: no need to confirm hits, just play vfx on all clients
	InstantHitConfirmed(HitResult,ShotDirection);
}


void AZWeapon_HitScan::InstantHitConfirmed(FHitResult& HitResult, FVector ShotDirection)
{
	if(HasAuthority())
	{
		if(HitResult.GetActor() && HitResult.GetActor()->HasAuthority())
		{
			DoDamage(HitResult, ShotDirection);
		}
		
		HitScanImpactNotify.BurstCounter+=1;
		HitScanImpactNotify.ImpactLocation = HitResult.Location;
	}
	if(GetNetMode()!=NM_DedicatedServer)
	{
		SimulateWeaponFire(HitResult.Location);
	}
}


void AZWeapon_HitScan::OnRep_HitScanImpactNotify()
{
	SimulateWeaponFire(HitScanImpactNotify.ImpactLocation);
}


void AZWeapon_HitScan::ServerNotifyHit_Implementation(FHitResult HitResult, FVector_NetQuantizeNormal ShotDirection)
{
	if(OwnerPlayer && (HitResult.bBlockingHit || HitResult.GetActor()))
	{
		if(!bIsReloading)
		{
			// Check dot between view and shot direction
			if(HitDirectionAcceptable(HitResult.Location))
			{
				if(HitResult.GetActor()==nullptr)
				{
					if(HitResult.bBlockingHit)
					{
						InstantHitConfirmed(HitResult, ShotDirection);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("(Server) ServerNotifyHit is the wrong function call. Should have called ServerNotifyMiss"))
					}
				}
				
				// If hit actor is stationary, it doesn't impact gameplay
				else if(HitResult.GetActor()->IsRootComponentStatic() || HitResult.GetActor()->IsRootComponentStationary())
				{
					InstantHitConfirmed(HitResult, ShotDirection);
				}
				
				// Hit actor was dynamic and shot needs to be checked
				else
				{
					// Get HitBox of Hit Actor
					const FBox HitBox = HitResult.GetActor()->GetComponentsBoundingBox();

					// BoxExtent is vector from HitBox.Min to center. Then multiply it by a leeway
					FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
					BoxExtent *= ClientSideHitLeeway;

					// Box Center
					const FVector BoxCenter = 0.5 * (HitBox.Max + HitBox.Min);
					
					BoxExtent.X = FMath::Max(20.f, BoxExtent.X);
					BoxExtent.Y = FMath::Max(20.f, BoxExtent.Y);
					BoxExtent.Z = FMath::Max(20.f, BoxExtent.Z);

					if(FMath::Abs(HitResult.Location.X - BoxCenter.X) < BoxExtent.X &&
					FMath::Abs(HitResult.Location.Y - BoxCenter.Y) < BoxExtent.Y &&
					FMath::Abs(HitResult.Location.Z - BoxCenter.Z) < BoxExtent.Z)
					{
						InstantHitConfirmed(HitResult,ShotDirection);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("(Server) %s Rejected client side hit of %s: Hit outside hitbox tolerance"),*GetNameSafe(this),*GetNameSafe(HitResult.GetActor()));
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("(Server) %s Rejected client side hit of %s: Facing too far away from hit direction"),*GetNameSafe(this),*GetNameSafe(HitResult.GetActor()));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("(Server) Rejected client side hit: Weapon state mismatch"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("(Server) This log shouldn't be triggered. Check ServerNotifyHit uses"));
	}
}


bool AZWeapon_HitScan::ServerNotifyHit_Validate(FHitResult HitResult, FVector_NetQuantizeNormal ShotDirection)
{
	return true;
}


void AZWeapon_HitScan::ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShotDirection)
{
	if(OwnerPlayer)
	{
		HitScanImpactNotify.BurstCounter+=1;
		HitScanImpactNotify.ImpactLocation = MeshComp->GetSocketLocation(MuzzleSocketName) + (ShotDirection * WeaponRange);
	}
}


bool AZWeapon_HitScan::PerformWeaponTrace(FHitResult& HitResult, FVector TraceStartLocation, FVector TraceEndLocation) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(OwnerPlayer);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	const bool BlockingHit = GetWorld()->LineTraceSingleByChannel(HitResult,TraceStartLocation, TraceEndLocation, COLLISION_WEAPON, QueryParams);

	return BlockingHit;
}


bool AZWeapon_HitScan::HitDirectionAcceptable(FVector& ShotEndLocation) const
{
	const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	const FVector ViewDir = (ShotEndLocation - MuzzleLocation).GetSafeNormal();
	const float ViewDotShotDir = FVector::DotProduct(GetInstigator()->GetViewRotation().Vector(),ViewDir);

	if(ViewDotShotDir > AllowedViewDotHitDir)
		return true;
	
	return false;
}


void AZWeapon_HitScan::SimulateWeaponFire(FVector ImpactLocation)
{
	const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	FHitResult SimulatedHitResult;

	const bool SimulatedHit = PerformWeaponTrace(SimulatedHitResult,MuzzleLocation,ImpactLocation+(ImpactLocation-MuzzleLocation).GetSafeNormal());
	
	if(SimulatedHit)
	{
		PlayImpactEffects(SimulatedHitResult.Location,SimulatedHitResult.PhysMaterial->SurfaceType);
	}
	PlayTrailEffects(SimulatedHitResult.Location);
}


void AZWeapon_HitScan::PlayImpactEffects(FVector ImpactLocation, EPhysicalSurface ImpactSurface) const
{
	UParticleSystem* EffectToPlay = nullptr;
	switch (ImpactSurface)
	{
	case SURFACE_PAWN_HEAD:
    case SURFACE_PAWN_BODY:
    case SURFACE_PAWN_ARM:
    case SURFACE_PAWN_LEG:
        if(BloodImpactEffect)
        	EffectToPlay = BloodImpactEffect;
		break;
		
	case SurfaceType_Default:
	default:
        if(DefaultImpactEffect)
        	EffectToPlay = DefaultImpactEffect;
		break;
	}
	if(EffectToPlay)
	{
		FVector ShotDirection = ImpactLocation - MeshComp->GetSocketLocation(MuzzleSocketName);
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(this,EffectToPlay,ImpactLocation,ShotDirection.Rotation());
	}
}


void AZWeapon_HitScan::PlayTrailEffects(FVector ImpactLocation)
{
	if(!TracerEffect)
		return;
	
	ShotCount++;
		
	const FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
	const float ShotDistance = (ImpactLocation - MuzzleLocation).Size();
	
	if(ShotDistance > MinimumDistanceToSpawnTracer)
	{
		if(ShotCount % TracerRoundInterval == 0)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this,TracerEffect,MuzzleLocation,(ImpactLocation-MuzzleLocation).Rotation());
		}
	}
}


void AZWeapon_HitScan::DoDamage(FHitResult& HitResult, FVector& ShotDirection)
{
	float Damage = DamagePerShot;

	switch(HitResult.PhysMaterial->SurfaceType)
	{
	case SURFACE_PAWN_HEAD:
		Damage*=HeadShotDamageMultiplier;
		break;
	case SURFACE_PAWN_BODY:
		Damage*=BodyShotDamageMultiplier;
		break;
	case SURFACE_PAWN_ARM:
		Damage*=ArmShotDamageMultiplier;
		break;
	case SURFACE_PAWN_LEG:
		Damage*=LegShotDamageMultiplier;
		break;
	default:
		break;
	}

	UGameplayStatics::ApplyPointDamage(HitResult.GetActor(),Damage,ShotDirection,HitResult,OwnerPlayer->GetController(),this,DamageTypeClass);
}


void AZWeapon_HitScan::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AZWeapon_HitScan, HitScanImpactNotify, COND_SkipOwner);
	
}
