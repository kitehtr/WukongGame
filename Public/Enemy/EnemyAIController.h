// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	explicit AEnemyAIController(FObjectInitializer const& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn) override;


protected:
	virtual void BeginPlay() override;


private:
	class UAISenseConfig_Sight* SightConfig;

	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetFound(AActor* Actor, FAIStimulus const Stimulus);


};
