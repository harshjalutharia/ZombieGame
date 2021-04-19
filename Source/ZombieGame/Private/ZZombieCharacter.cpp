// Fill out your copyright notice in the Description page of Project Settings.


#include "ZZombieCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ZHealthComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AZZombieCharacter::AZZombieCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UZHealthComponent>("HealthComp");
	if(HealthComp)
	{
		HealthComp->SetIsReplicated(true);
	}

	AutoPossessAI=EAutoPossessAI::PlacedInWorldOrSpawned;
	
}

// Called when the game starts or when spawned
void AZZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		AAIController* AIController = Cast<AAIController>(GetController());
		AIController->RunBehaviorTree(ZombieBehaviorTree);

		HealthComp->OnHealthZero.AddDynamic(this, &AZZombieCharacter::OnCharacterDeath);
	}
}

// Called every frame
void AZZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


void AZZombieCharacter::OnCharacterDeath(UZHealthComponent* HealthComponent, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("ZOMBIE DIED"))
	
	if(HasAuthority())
		UE_LOG(LogTemp, Warning, TEXT("SERVER"))
	else
		UE_LOG(LogTemp, Warning, TEXT("CLIENT"))

	GetController()->StopMovement();
	GetController()->UnPossess();

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);

	/*GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetCapsuleComponent()->SetCollisionProfileName("Ragdoll");*/
}
