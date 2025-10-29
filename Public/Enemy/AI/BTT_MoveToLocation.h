// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MoveToLocation.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTT_MoveToLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_MoveToLocation(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptableRadius = 150.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float PathUpdateThreshold = 200.0f; 

	UPROPERTY(EditAnywhere, Category = "AI")
	float MinUpdateInterval = 0.25f; 

private:
	FVector LastTargetLocation = FVector::ZeroVector;
	float TimeSinceLastUpdate = 0.0f;
	uint32 MoveRequestID = 0;
};
