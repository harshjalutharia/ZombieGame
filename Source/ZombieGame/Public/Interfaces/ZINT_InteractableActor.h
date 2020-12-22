// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ZINT_InteractableActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UZINT_InteractableActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIEGAME_API IZINT_InteractableActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact(class APlayerCharacter* PlayerInteracted);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldHighlightWhenLookedAt();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartHighlightingActor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopHighlightingActor();
};
