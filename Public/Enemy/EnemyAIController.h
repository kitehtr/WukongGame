// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();
	explicit AEnemyAIController(FObjectInitializer const& ObjectInitializer);


protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
};
