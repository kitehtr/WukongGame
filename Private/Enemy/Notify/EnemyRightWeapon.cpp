// Fill out your copyright notice in the Description page of Project Settings.

#include "../../../../WukongGame/Public/Enemy/Notify/EnemyRightWeapon.h"
#include "../../../../WukongGame/Public/Enemy/EnemyMelee.h"

void UEnemyRightWeapon::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		MeleeEnemy = Cast<AEnemyMelee>(MeshComp->GetOwner());
		if (MeleeEnemy)
		{
			MeleeEnemy->ActivateRightWeapon();
		}
	}
}

void UEnemyRightWeapon::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		MeleeEnemy = Cast<AEnemyMelee>(MeshComp->GetOwner());
		if (MeleeEnemy)
		{
			MeleeEnemy->DeactivateRightWeapon();
		}
	}
}
