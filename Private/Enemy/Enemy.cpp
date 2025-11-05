// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Enemy/Enemy.h"
#include "../../Public/Enemy/EnemyAIController.h"
#include "../../Public/Character/MyWukongCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy() :
	Health(100.0f),
	MaxHealth(100.0f)

{
	PrimaryActorTick.bCanEverTick = true;

}

APatrolPath* AEnemy::GetPatrolPath() const
{
	if (!IsValid(PatrolPath))
	{
		return nullptr;
	}
	return PatrolPath;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add("Enemy");
	EnemyAIController = Cast <AEnemyAIController> (GetController());
}

void AEnemy::StartStun(float Duration)
{
	if (bIsStunned) return;

	bIsStunned = true;
	bCanBeStunned = false;

	if (EnemyAIController && EnemyAIController->GetBrainComponent())
	{
		EnemyAIController->GetBrainComponent()->StopLogic("Stunned");
	}

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->DisableMovement();
	}

	GetWorld()->GetTimerManager().SetTimer(StunTimerHandle,this,&AEnemy::EndStun,Duration,false);
}

void AEnemy::EndStun()
{
	if (!bIsStunned) return;

	bIsStunned = false;

	UE_LOG(LogTemp, Warning, TEXT("%s: Stun ended"), *GetName());

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->SetMovementMode(MOVE_Walking);
	}

	if (EnemyAIController && EnemyAIController->GetBrainComponent())
	{
		EnemyAIController->GetBrainComponent()->RestartLogic();
	}

	if (!IsValid(this))
	{
		return;
	}

	TWeakObjectPtr<AEnemy> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(StunCooldownTimerHandle, [WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->bCanBeStunned = true;
			}
		}, StunCooldown, false);
}

FVector AEnemy::GetHitDirection(AActor* DamageCauser)
{
	if (!DamageCauser) return FVector::ForwardVector;

	FVector DirectionToAttacker = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	FVector LocalDirection = GetActorRotation().UnrotateVector(DirectionToAttacker);
	LocalDirection.Z = 0; 

	return LocalDirection.GetSafeNormal();
}

void AEnemy::PlayDirectionalHitReaction(const FVector& HitDirection)
{
	if (!GetMesh() || !GetMesh()->GetAnimInstance()) return;

	UAnimMontage* MontageToPlay = nullptr;

	float ForwardDot = FVector::DotProduct(HitDirection, FVector::ForwardVector);
	float RightDot = FVector::DotProduct(HitDirection, FVector::RightVector);

	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		if (ForwardDot > 0)
		{
			MontageToPlay = HitReactionFrontMontage;
		}
		else
		{
			MontageToPlay = HitReactionBackMontage;;
		}
	}
	else
	{
		if (RightDot > 0)
		{
			MontageToPlay = HitReactionRightMontage;
		}
		else
		{
			MontageToPlay = HitReactionLeftMontage;
			UE_LOG(LogTemp, Log, TEXT("Hit from LEFT"));
		}
	}

	if (MontageToPlay)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::MeleeHit_Implementation(FHitResult HitResult)
{

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return DamageAmount; 

	if (Health - DamageAmount <= 0.0f)
	{
		bIsDead = true; 
		Health = 0.0f;
		bIsStunned = false;

		if (EnemyAIController)
		{
			EnemyAIController->GetBrainComponent()->StopLogic("Dead");
			if (GetWorld()->GetFirstPlayerController())
			{
				AMyWukongCharacter* Wukong = Cast<AMyWukongCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
				if (Wukong)
				{
					Wukong->AddEnemyDefeated();
				}
			}
		}
		EnemyDeath();
	}
	else
	{
		Health -= DamageAmount;
		if (DamageCauser)
		{
			FVector HitDirection = GetHitDirection(DamageCauser);
			PlayDirectionalHitReaction(HitDirection);
			PlayHitReaction(HitDirection);

			if (bCanBeStunned && !bIsStunned)
			{
				StartStun(StunDuration);
			}
		}
	}

	return DamageAmount;
}
