// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/Notify/AnimNotify_FireProjectile.h"
#include "Project/WukongGame/Public/Enemy/RangedEnemy.h"


void UAnimNotify_FireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::Notify(MeshComp, Animation);

    if (!MeshComp || !MeshComp->GetOwner())
    {
        return;
    }

    ARangedEnemy* RangedEnemy = Cast<ARangedEnemy>(MeshComp->GetOwner());
    if (RangedEnemy)
    {
        RangedEnemy->FireProjectile();
    }

}
