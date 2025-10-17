// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTT_FindPathPoint.h"

UBTT_FindPathPoint::UBTT_FindPathPoint(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase{ ObjectInitializer }
{
    NodeName = ("Find Path Point");
}

EBTNodeResult::Type UBTT_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (auto* const AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner()))
    {
        if (auto* const BlackboardComp = OwnerComp.GetBlackboardComponent())
        {
            if (auto* enemy = Cast<AEnemy>(AIController->GetPawn()))
            {
                APatrolPath* PatrolPath = enemy->GetPatrolPath();
                if (!IsValid(PatrolPath))
                {
                    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
                    return EBTNodeResult::Failed;
                }

                auto const index = BlackboardComp->GetValueAsInt(GetSelectedBlackboardKey());
                int32 NumPoints = PatrolPath->Num();
                if (index < 0 || index >= NumPoints)
                {
                    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
                    return EBTNodeResult::Failed;
                }

                auto const Point = PatrolPath->GetPatrolPoint(index);
                auto const GlobalPoint = PatrolPath->GetActorTransform().TransformPosition(Point);

                BlackboardComp->SetValueAsVector(PatrolPathVectorkey.SelectedKeyName, GlobalPoint);

                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                return EBTNodeResult::Succeeded;
            }
        }
    }

    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    return EBTNodeResult::Failed;
}