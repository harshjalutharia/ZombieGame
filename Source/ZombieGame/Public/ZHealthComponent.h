// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedSignature, class UZHealthComponent*, HealthComponent, float, Health, float, HealthDelta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthZeroSignature, class UZHealthComponent*, HealthComponent, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIEGAME_API UZHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UZHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	float DefaultHealth;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	float CurrentHealth;

	UFUNCTION()
	void OnRep_CurrentHealth(float OldHealth);

public:

	UPROPERTY()
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY()
	FOnHealthZeroSignature OnHealthZero;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	float GetMaxHealth() const { return DefaultHealth; }
	float GetHealth() const { return CurrentHealth; }
	
};
