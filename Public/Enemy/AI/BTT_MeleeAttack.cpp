// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTT_MeleeAttack.h"
#include "../../../Public/Enemy/MeleeHitInterface.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "../../../Public/Enemy/EnemyMelee.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Engine/LatentActionManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

void UBTT_MeleeAttack::BTT_MeleeAttack()
{
	NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto const OutOfRange = !OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
    if (OutOfRange)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return EBTNodeResult::Succeeded;
    }

    auto const* const AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
    auto* const enemy = Cast<AEnemyMelee>(AIController->GetPawn());

    if (enemy)
    {
        enemy->MainMeleeAttack(); 
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&OwnerComp, this]()
            {FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);}, 2.0f, false); 
    }

     return EBTNodeResult::InProgress;
}

bool UBTT_MeleeAttack::MontageHasFinished(AEnemy* const enemy, UAnimMontage* Montage)
{
	if (!enemy || !Montage) return true;

	if (UAnimInstance* AnimInstance = enemy->GetMesh()->GetAnimInstance())
	{
		return AnimInstance->Montage_GetIsStopped(Montage);
	}

	return true;
}
