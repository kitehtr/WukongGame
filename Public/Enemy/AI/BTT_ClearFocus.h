// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_ClearFocus.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTT_ClearFocus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_ClearFocus();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	bool bClearFocusOnly = false;

};
