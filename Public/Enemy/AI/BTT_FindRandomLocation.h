// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTT_FindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTT_FindRandomLocation(FObjectInitializer const&);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float SearchRadius = 1500.0f;
	
};
