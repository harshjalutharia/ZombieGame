// Fill out your copyright notice in the Description page of Project Settings.


#include "ZInteractableActor.h"
#include "PlayerCharacter.h"


// Sets default values
AZInteractableActor::AZInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bHighlightWhenLookedAt = false;
}


void AZInteractableActor::StartHighlightingActor()
{
	UE_LOG(LogTemp, Warning, TEXT("Actor is being highlighted"));
	//MeshComp->SetRenderCustomDepth(true);
}


void AZInteractableActor::StopHighlightingActor()
{
	UE_LOG(LogTemp, Warning, TEXT("Actor stopped being highlighted"));
}

