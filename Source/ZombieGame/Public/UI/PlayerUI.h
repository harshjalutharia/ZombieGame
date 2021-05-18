// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/ZINT_PlayerHUD.h"
#include "PlayerUI.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;
class APlayerCharacter;
class AZWeapon;
class UZHealthComponent;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UPlayerUI : public UUserWidget, public IZINT_PlayerHUD
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere)
	APlayerCharacter* OwnerPlayer;

	UPROPERTY(VisibleAnywhere)
	UZHealthComponent* HealthComp;

	float MaxHealth;

protected:

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ClipAmmoText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ReserveAmmoText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AmmoDivider;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta=(BindWidget))
	UImage* ActiveWeaponImage;

	UPROPERTY(BlueprintReadOnly)
	class AZWeapon* LastActiveWeapon;

	// Animation being set in WBP graph
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetAnimation* CrosshairRecoilAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetAnimation* WeaponEquipAnimation;

	void PlayWeaponEquipAnimation();

public:

	virtual void PlayCrosshairRecoil() override;
	
};
