// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZInteractableActor.generated.h"

class APlayerCharacter;

UCLASS()
class ZOMBIEGAME_API AZInteractableActor : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Configuration")
	bool bHighlightWhenLookedAt;

public:	
	// Sets default values for this actor's properties
	AZInteractableActor();

	virtual void Interact(APlayerCharacter* PlayerInteracted) PURE_VIRTUAL(AZInteractableActor::Interact, ;);

	virtual void StartHighlightingActor();

	virtual void StopHighlightingActor();

};
