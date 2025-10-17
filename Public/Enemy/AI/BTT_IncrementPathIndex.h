// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../../Public/Enemy/Enemy.h"
#include "BTT_IncrementPathIndex.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTT_IncrementPathIndex : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	explicit UBTT_IncrementPathIndex(FObjectInitializer const&);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	enum class EDirectionType {Forward, Reverse};

	EDirectionType Direction = EDirectionType::Forward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	bool bisBiDirectional = false;
};
