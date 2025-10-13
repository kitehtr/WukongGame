// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Enemy/EnemyAIController.h"
#include "../../Public/Enemy/Enemy.h"

AEnemyAIController::AEnemyAIController()
{

}

AEnemyAIController::AEnemyAIController(FObjectInitializer const& ObjectInitializer)
{

}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (AEnemy* const Enemy = Cast<AEnemy>(InPawn))
    {
        if (UBehaviorTree* const Tree = Enemy->GetBehaviorTree())
        {
            UBlackboardComponent* BlackboardComp = nullptr;

            if (UseBlackboard(Tree->BlackboardAsset, BlackboardComp))
            {
                Blackboard = BlackboardComp;
                RunBehaviorTree(Tree);
            }
        }
    }
}
