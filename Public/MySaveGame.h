// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMySaveGame();
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	FVector PlayerLocation;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	FRotator PlayerRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	bool SGHasStartedGame = false;
	
};
