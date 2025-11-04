// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/Ranged/BTT_RangedAttack.h"
#include "../../../../Public/Enemy/EnemyAIController.h"
#include "Project/WukongGame/Public/Enemy/RangedEnemy.h"

UBTT_RangedAttack::UBTT_RangedAttack()
{
	NodeName = "Ranged Attack";
}

EBTNodeResult::Type UBTT_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    ARangedEnemy* RangedEnemy = Cast<ARangedEnemy>(AIController->GetPawn());
    if (!RangedEnemy)
    {
        return EBTNodeResult::Failed;
    }

    if (RangedEnemy->CanAttack())
    {
        RangedEnemy->RangedAttack();
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
