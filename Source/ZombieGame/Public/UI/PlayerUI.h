// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUI.generated.h"

class UTextBlock;
class UProgressBar;
class APlayerCharacter;
class AZWeapon;
class UZHealthComponent;

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UPlayerUI : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	APlayerCharacter* OwnerPlayer;

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

};
