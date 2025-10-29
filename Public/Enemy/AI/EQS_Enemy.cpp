// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/EQS_Enemy.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "BehaviorTree/BlackboardComponent.h"

void UEQS_Enemy::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AEnemyAIController* Controller = Cast<AEnemyAIController>((Cast<AActor>((QueryInstance.Owner).Get())->GetInstigatorController()));
	if (Controller)
	{
		if (UBlackboardComponent* BlackboardComp = Controller->GetBlackboardComponent())
		{
			AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
			if (Target)
			{
				UEnvQueryItemType_Actor::SetContextHelper(ContextData, Target);
			}
		}
	}
}
