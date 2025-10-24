// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_IsPlayerWithinMeleeRange.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTS_IsPlayerWithinMeleeRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	void UBTS_IsPlayerInMeleeRange();
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess="true"))
	float MeleeRange = 2.5f;
};
