// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "../../../Public/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BTT_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()

class WUKONG_API UBTT_MeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	void BTT_MeleeAttack();
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	bool MontageHasFinished(AEnemy* const enemy, UAnimMontage* Montage);
};
