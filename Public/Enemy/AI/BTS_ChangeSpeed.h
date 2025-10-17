// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "../../../Public/Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BTS_ChangeSpeed.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTS_ChangeSpeed : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	void UBTService_ChangeSpeed();
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI", meta=(AllowPrivateAccess="true"))
	float Speed = 600.0f;
	
};
