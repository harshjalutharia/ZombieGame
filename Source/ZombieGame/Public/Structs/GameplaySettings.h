
#pragma once

#include "GameplaySettings.generated.h"

USTRUCT()
struct FGameplaySettings
{
	GENERATED_BODY()

	UPROPERTY()
	float MouseSensitivity;

	UPROPERTY()
	bool bToggleADS;

	friend bool operator==(const FGameplaySettings& LHSSettings, const FGameplaySettings& RHSSettings);

	friend bool operator!=(const FGameplaySettings& LHSSettings, const FGameplaySettings& RHSSettings); 
};


inline bool operator==(const FGameplaySettings& LHSSettings, const FGameplaySettings& RHSSettings)
{
	if(LHSSettings.MouseSensitivity == RHSSettings.MouseSensitivity)
		if(LHSSettings.bToggleADS == RHSSettings.bToggleADS)
			return true;
	return false;
}


inline bool operator!=(const FGameplaySettings& LHSSettings, const FGameplaySettings& RHSSettings)
{
	return !(LHSSettings == RHSSettings);
}