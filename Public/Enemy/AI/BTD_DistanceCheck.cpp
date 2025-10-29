// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTD_DistanceCheck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "../../../Public/Enemy/EnemyAIController.h"

UBTD_DistanceCheck::UBTD_DistanceCheck(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Distance Check";
	bCreateNodeInstance = true;
	
}


bool UBTD_DistanceCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	bool OutsideRange = false;

	if (!MyBlackboard || !AIController)
	{
		return OutsideRange;
	}

	AEnemyAIController* EnemyController = Cast<AEnemyAIController>(AIController);
	if (!EnemyController)
	{
		return OutsideRange;
	}

	auto EnemyID = MyBlackboard->GetKeyID(BlackboardKey.SelectedKeyName);
	UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(EnemyID);
	AActor* EnemyActor = Cast<AActor>(KeyValue);

	if (!EnemyActor)
	{
		return OutsideRange;
	}

	APawn* ControlledPawn = EnemyController->GetPawn();
	if (!ControlledPawn)
	{
		return OutsideRange;
	}

	if (ControlledPawn->GetDistanceTo(EnemyActor) >= TestDistance)
	{
		OutsideRange = true;
	}

	return OutsideRange;
}
