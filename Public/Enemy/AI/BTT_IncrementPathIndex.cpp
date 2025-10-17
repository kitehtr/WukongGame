// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTT_IncrementPathIndex.h"

UBTT_IncrementPathIndex::UBTT_IncrementPathIndex(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = ("Increment Path Index");
}

EBTNodeResult::Type UBTT_IncrementPathIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (auto* const AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner()))
    {
        if (auto* const enemy = Cast<AEnemy>(AIController->GetPawn()))
        {
            APatrolPath* PatrolPath = enemy->GetPatrolPath();
            if (!IsValid(PatrolPath))
            {
                FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
                return EBTNodeResult::Failed;
            }

            if (auto* const BlackboardComp = OwnerComp.GetBlackboardComponent())
            {
                auto const NumOfPoints = PatrolPath->Num();

                if (NumOfPoints <= 0)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Patrol path has no points for enemy %s"), *enemy->GetName());
                    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
                    return EBTNodeResult::Failed;
                }

                auto const MinIndex = 0;
                auto const MaxIndex = NumOfPoints - 1;
                auto Index = BlackboardComp->GetValueAsInt(GetSelectedBlackboardKey());

                if (bisBiDirectional)
                {
                    if (Index >= MaxIndex && Direction == EDirectionType::Forward)
                    {
                        Direction = EDirectionType::Reverse;
                    }
                    else if (Index == MinIndex && Direction == EDirectionType::Reverse)
                    {
                        Direction = EDirectionType::Forward;
                    }
                }

                Index = (Direction == EDirectionType::Forward ? Index + 1 : Index - 1);

                if (Index < MinIndex) Index = MaxIndex;
                else if (Index > MaxIndex) Index = MinIndex;

                BlackboardComp->SetValueAsInt(GetSelectedBlackboardKey(), Index);
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                return EBTNodeResult::Succeeded;
            }
        }
    }

    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    return EBTNodeResult::Failed;
}
