// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZWeapon.h"
#include "ZombieGame/ZEnums.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UPlayerCharacterMovementComponent;
class UZHealthComponent;
class AZWeapon;


UCLASS()
class ZOMBIEGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter(const class FObjectInitializer& ObjectInitializer);

private:

	/*
	COMPONENTS
	*/

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* CameraBoomComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY()
	UPlayerCharacterMovementComponent* MoveComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UZHealthComponent* HealthComp;

	/*
	INPUT BINDING
	*/

	void MoveForward(float Value);
	void MoveRight(float Value);

	void StartCrouching();
	void StopCrouching();
	
	void StartSprinting();
	void StopSprinting();

	void StartAiming();
	void StopAiming();

	void RequestJump();

	void StartFiring();
	void StopFiring();

	void RequestReload();

	void QuickSwitchWeapon();

	void UseWeaponSlot1();
	void UseWeaponSlot2();

	void Interact();

	/*
	INTERACTION
	 */

	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	float InteractionRange;

	//AActor* FocusedActor;

	AActor* GetActorInView() const;
	
	/*
	MOVEMENT STUFF
	*/

	void SetLookPitch();

	UPROPERTY(Replicated)
	float LookPitch;

	UPROPERTY(Replicated)
	bool bIsAiming;

	UPROPERTY(Replicated)
	bool bWantsToSprint;

	bool bWantsToFire;

	UPROPERTY(Replicated)
	bool bIsJumping;

	UPROPERTY(Replicated)
	bool bIdleJump;

	void SetCrouching(bool NewCrouching);
	void SetSprinting(bool NewSprinting);
	void SetAiming(bool NewAiming);
	void SetJumping(bool NewJumping);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerSetSprinting(bool NewSprinting);
	void ServerSetSprinting_Implementation(bool NewSprinting);
	bool ServerSetSprinting_Validate(bool NewSprinting);
	
	UFUNCTION(Server, Reliable, WithValidation)
    void ServerSetAiming(bool NewAiming);
	void ServerSetAiming_Implementation(bool NewAiming);
	bool ServerSetAiming_Validate(bool NewAiming);
	
	UFUNCTION(Server, Reliable, WithValidation)
    void ServerSetJumping(bool NewJumping, bool IsIdleJump = false);
	void ServerSetJumping_Implementation(bool NewJumping, bool IsIdleJump = false);
	bool ServerSetJumping_Validate(bool NewJumping, bool IsIdleJump = false);

	/*
	 WEAPONS STUFF
	*/

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerSwitchWeapon(AZWeapon* NewActiveWeapon);
	void ServerSwitchWeapon_Implementation(AZWeapon* NewActiveWeapon);
	bool ServerSwitchWeapon_Validate(AZWeapon* NewActiveWeapon);

	UPROPERTY(Replicated)
	AZWeapon* WeaponSlot1;

	UPROPERTY(Replicated)
	AZWeapon* WeaponSlot2;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeapon)
	AZWeapon* ActiveWeapon;

	UPROPERTY(Replicated)
	AZWeapon* LastWeaponInHand;
	
	// Socket in Player Mesh where Weapon being held in hand will be attached
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	FName ActiveWeaponSocketName;

	//Socket in Player Mesh where Weapon in reserve will be attached
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	FName ReserveWeaponSlot1SocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	FName ReserveWeaponSlot2SocketName;

	UPROPERTY(EditAnywhere, Category = "Weapons")
	bool bSpawnWithWeapon;

	UPROPERTY(EditAnywhere, Category = "Weapons", meta = (EditCondition = "bSpawnWithWeapon"))
	TSubclassOf<AZWeapon> SpawnWithWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	bool bAutoSwitchNewWeapon;

	EWeaponState ActiveWeaponState;

	UFUNCTION()
	void OnRep_ActiveWeapon();
	
	EInventorySlot GetFirstEmptyWeaponSlot() const;

	void ClearWeaponSlot(EInventorySlot WeaponSlotIndex);

	void AddWeaponToInventory(AZWeapon* NewWeapon, EInventorySlot EmptySlot);

	UFUNCTION(Server, Reliable, WithValidation)
    void ServerAddWeaponToInventory(AZWeapon* NewWeapon, EInventorySlot EmptySlot);
	void ServerAddWeaponToInventory_Implementation(AZWeapon* NewWeapon, EInventorySlot EmptySlot);
	bool ServerAddWeaponToInventory_Validate(AZWeapon* NewWeapon, EInventorySlot EmptySlot);

	void AssignWeaponToSlot(AZWeapon* WeaponToAssign, EInventorySlot Slot);

	UFUNCTION(BlueprintCallable)
    bool HoldingWeapon() const { return (ActiveWeapon != nullptr); }

public:
	
	FName GetAttachPointOfSlot(EInventorySlot Slot) const;
	FName GetHandAttachPoint() const { return ActiveWeaponSocketName; }

	UFUNCTION(BlueprintCallable)
	AZWeapon* GetActiveWeapon() const;
	AZWeapon* GetLastWeaponInHand() const { return LastWeaponInHand; }

	void TryPickupWeapon(AZWeapon* NewWeapon);

	void SetActiveWeaponState(EWeaponState NewWeaponState);
	
	bool CanFire() const { return !IsJumping(); }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	UFUNCTION(BlueprintCallable)
	float GetLookPitch() const { return LookPitch; }
	UFUNCTION(BlueprintCallable)
	bool IsAiming() const { return bIsAiming; }
	UFUNCTION(BlueprintCallable)
	bool IsSprinting() const;
	UFUNCTION(BlueprintCallable)
	bool IsJumping() const { return bIsJumping; }
	UFUNCTION(BlueprintCallable)
    bool IsIdleJump() const { return bIdleJump; }

	UCameraComponent* GetCameraComponent() const { return CameraComp; }
	UZHealthComponent* GetHealthComponent() const { return HealthComp; }
	
};
