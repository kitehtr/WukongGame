// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_DistanceCheck.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTD_DistanceCheck : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UBTD_DistanceCheck(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "Condition")
	float TestDistance = 1000.0f;
};
