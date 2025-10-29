// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_UpdateTargetLocation.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTS_UpdateTargetLocation : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTS_UpdateTargetLocation();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector TargetActorKey;
};
