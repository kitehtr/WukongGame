// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_IsStunned.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTD_IsStunned : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTD_IsStunned();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual void OnBecomeRelevant(AAIController& OwnerController, uint8* NodeMemory);
	
};
