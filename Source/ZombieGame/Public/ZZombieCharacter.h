// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZZombieCharacter.generated.h"

UENUM()
enum class EZombieState : uint8
{
	Idle,
	Wandering,
	Running,
	Attacking
};

UCLASS()
class ZOMBIEGAME_API AZZombieCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZZombieCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UZHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	class UBehaviorTree* ZombieBehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = "Zombie")
	EZombieState ZombieState;

	UPROPERTY(VisibleAnywhere, Category = "Zombie")
	bool IsDead;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCharacterDeath(class UZHealthComponent* HealthComponent, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

};
