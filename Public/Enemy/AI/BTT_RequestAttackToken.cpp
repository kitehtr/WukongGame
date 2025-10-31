// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTT_RequestAttackToken.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "../../../Public/Enemy/EnemyMelee.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_RequestAttackToken::UBTT_RequestAttackToken()
{
	NodeName = "Request Attack Token";
}

EBTNodeResult::Type UBTT_RequestAttackToken::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AEnemyMelee* Enemy = Cast<AEnemyMelee>(AIController->GetPawn());
	if (!Enemy)
	{
		return EBTNodeResult::Failed;
	}

	bool bGotToken = Enemy->RequestAttackToken();

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->SetValueAsBool("HasAttackToken", bGotToken);
	}

	return bGotToken ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
