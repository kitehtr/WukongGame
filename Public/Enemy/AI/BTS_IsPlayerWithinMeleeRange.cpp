// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTS_IsPlayerWithinMeleeRange.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "../../../Public/Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "../../../Public/Character/MyWukongCharacter.h"
#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTS_IsPlayerWithinMeleeRange::UBTS_IsPlayerInMeleeRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Player in Melee Range");
}

void UBTS_IsPlayerWithinMeleeRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    auto const* const AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return;

    auto const* const enemy = Cast<AEnemy>(AIController->GetPawn());
    if (!enemy) return;

    auto const* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!Player) return;

    OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), enemy->GetDistanceTo(Player) <= MeleeRange);
}