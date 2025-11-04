// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/RangedEnemyAnimInstance.h"
#include "Project/WukongGame/Public/Enemy/RangedEnemy.h"
#include "Kismet/KismetMathLibrary.h"

void URangedEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
		if (Enemy == nullptr)
		{
			return;
		}
	}

	if (Enemy)
	{
		FVector Velocity{ Enemy->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();
	}

	const FRotator AimRotation = Enemy->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Enemy->GetVelocity());

	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
}