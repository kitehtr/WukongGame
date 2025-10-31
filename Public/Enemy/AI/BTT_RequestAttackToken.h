// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RequestAttackToken.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTT_RequestAttackToken : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_RequestAttackToken();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Token")
	float RetryDelay = 1.0f;
};
