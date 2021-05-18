// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerUI.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "ZombieGame/Public/PlayerCharacter.h"
#include "ZHealthComponent.h"
#include "ZWeapon.h"


bool UPlayerUI::Initialize()
{
	const bool Success = Super::Initialize();
	if(!Success)
		return false;

	if(!HealthText || !HealthBar || !ClipAmmoText || !ReserveAmmoText || !AmmoDivider /*|| !ActiveWeaponImage*/)
		return false;
	
	OwnerPlayer = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if(OwnerPlayer)
	{
		HealthComp = OwnerPlayer->GetHealthComponent();
		if(HealthComp)
		{
			MaxHealth = HealthComp->GetMaxHealth();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerHUD OwnerPlayer does not have a HealthComponent"))
			return false;
		}	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerHUD OwnerPlayer not inititalized at Initialize()"))
		return false;
	}

	return true;
}

void UPlayerUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if(HealthComp)
	{
		HealthText->SetText(FText::FromString(FString::SanitizeFloat(HealthComp->GetHealth())));
		HealthBar->SetPercent(HealthComp->GetHealth()/MaxHealth);
	}

	AZWeapon* ActiveWeapon = OwnerPlayer->GetActiveWeapon();

	if(ActiveWeapon != nullptr)
	{
		if(LastActiveWeapon == nullptr || LastActiveWeapon != ActiveWeapon)
		{
			LastActiveWeapon = ActiveWeapon;
			
			ClipAmmoText->SetVisibility(ESlateVisibility::Visible);
			ReserveAmmoText->SetVisibility(ESlateVisibility::Visible);
			AmmoDivider->SetVisibility(ESlateVisibility::Visible);

			if(ActiveWeapon->GetWeaponImageUI())
			{
				ActiveWeaponImage->SetBrushFromTexture(ActiveWeapon->GetWeaponImageUI());
				ActiveWeaponImage->SetVisibility(ESlateVisibility::Visible);
				PlayWeaponEquipAnimation();
			}
			else
			{
				ActiveWeaponImage->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		
		ClipAmmoText->SetText(FText::FromString(FString::FromInt(ActiveWeapon->GetCurrentClipAmmo())));
		ReserveAmmoText->SetText(FText::FromString(FString::FromInt(ActiveWeapon->GetCurrentReserveAmmo())));
	}
	
	else
	{
		LastActiveWeapon = nullptr;
		
		ClipAmmoText->SetVisibility(ESlateVisibility::Hidden);
		ReserveAmmoText->SetVisibility(ESlateVisibility::Hidden);
		AmmoDivider->SetVisibility(ESlateVisibility::Hidden);
		ActiveWeaponImage->SetVisibility(ESlateVisibility::Hidden);
	}

	Super::NativeTick(MyGeometry, InDeltaTime);
}


void UPlayerUI::PlayWeaponEquipAnimation()
{
	if(WeaponEquipAnimation)
	{
		PlayAnimation(WeaponEquipAnimation);
	}
}


void UPlayerUI::PlayCrosshairRecoil()
{
	if(CrosshairRecoilAnimation)
	{
		PlayAnimation(CrosshairRecoilAnimation);
	}
}
