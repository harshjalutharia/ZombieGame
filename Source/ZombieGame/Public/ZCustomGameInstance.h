// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ZCustomGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEGAME_API UZCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UZCustomGameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void LoadPlayerHUD();

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable)
    void LoadLoadingScreen();

	UFUNCTION(BlueprintCallable)
    void LoadPauseMenu();

private:

	TSubclassOf<UUserWidget> PlayerHUDClass;

	TSubclassOf<UUserWidget> MainMenuClass;

	TSubclassOf<UUserWidget> LoadingScreenClass;

	TSubclassOf<UUserWidget> PauseMenuClass;
};
