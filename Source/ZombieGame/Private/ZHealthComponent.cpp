// Fill out your copyright notice in the Description page of Project Settings.


#include "ZHealthComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UZHealthComponent::UZHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	DefaultHealth = 100.f;
}


// Called when the game starts
void UZHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwnerRole()==ROLE_Authority)
	{
		AActor* OwnerActor = GetOwner();
		if(OwnerActor)
		{
			GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UZHealthComponent::OnTakeDamage);
		}
		
		CurrentHealth = DefaultHealth;
	}
}


void UZHealthComponent::OnRep_CurrentHealth(float OldHealth)
{
	const float Damage = OldHealth-CurrentHealth;
	OnHealthChanged.Broadcast(this,CurrentHealth, Damage);
}


void UZHealthComponent::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
									AController* InstigatedBy, AActor* DamageCauser)
{
	if(Damage==0.f)
		return;

	const float DamageToApply = CurrentHealth - FMath::Clamp(CurrentHealth-Damage,0.f,DefaultHealth);

	if (DamageToApply == 0.f)
		return;
	
	CurrentHealth -= DamageToApply;

	OnHealthChanged.Broadcast(this,CurrentHealth, DamageToApply);

	if(CurrentHealth==0.f)
	{
		OnHealthZero.Broadcast(this,DamageToApply,DamageType,InstigatedBy,DamageCauser);
	}
}


void UZHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UZHealthComponent, CurrentHealth);
}
