// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_FocusTarget.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTS_FocusTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTS_FocusTarget();

	virtual void OnBecomeRelevant(AAIController* OwnerController, uint8* NodeMemory);
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(AAIController* OwnerController, uint8* NodeMemory);

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	bool bClearFocusOnExit = false;
};
