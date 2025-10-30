// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTD_IsStunned.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "../../../Public/Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsStunned::UBTD_IsStunned()
{
	NodeName = "Is Stunned";
	bNotifyBecomeRelevant = true;
	bNotifyTick = false;
}

bool UBTD_IsStunned::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	AEnemy* Enemy = Cast<AEnemy>(AIController->GetPawn());
	if (!Enemy) return false;

	return Enemy->IsStunned();
}

void UBTD_IsStunned::OnBecomeRelevant(AAIController& OwnerController, uint8* NodeMemory)
{
	if (AEnemy* Enemy = Cast<AEnemy>(OwnerController.GetPawn()))
	{
		if (UBlackboardComponent* BB = OwnerController.GetBlackboardComponent())
		{
			BB->SetValueAsBool("IsStunned", Enemy->IsStunned());
		}
	}
}
