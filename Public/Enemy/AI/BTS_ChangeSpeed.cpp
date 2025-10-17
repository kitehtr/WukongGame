// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTS_ChangeSpeed.h"

void UBTS_ChangeSpeed::UBTService_ChangeSpeed()
{
	bNotifyBecomeRelevant = true;
	NodeName = ("Change Speed");
}

void UBTS_ChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	if (auto const AIController = OwnerComp.GetAIOwner())
	{
		if (auto* const enemy = Cast<AEnemy>(AIController->GetPawn()))
		{
			enemy->GetCharacterMovement()->MaxWalkSpeed = Speed;
		}
	}
}
