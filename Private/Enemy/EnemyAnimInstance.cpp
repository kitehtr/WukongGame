// Fill out your copyright notice in the Description page of Project Settings.



#include "../../Public/Enemy/EnemyAnimInstance.h"
#include "../../Public/Enemy/Enemy.h"
#include "Kismet/KismetMathLibrary.h"

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
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
