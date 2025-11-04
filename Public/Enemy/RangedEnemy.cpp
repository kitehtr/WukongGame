// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/RangedEnemy.h"
#include "../../Public/Enemy/EnemyAIController.h"
//#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Public/Character/MyWukongCharacter.h"
#include "DrawDebugHelpers.h"

ARangedEnemy::ARangedEnemy()
{
	LastAttackTime = -AttackCooldown;
}

void ARangedEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ARangedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsStunned || Health <= 0.0f)
		return;

	FindTarget();

	if (bIsCharging)
	{
		ShowChargingEffect();
	}
}

void ARangedEnemy::RangedAttack()
{
	if (!CanAttack())
	{
		return;
	}

	bIsCharging = true;
	ChargeStartTime = GetWorld()->GetTimeSeconds();

	GetWorld()->GetTimerManager().SetTimer(
		ChargeTimer,
		[this]() {
			PlayAttackAnimation();
		},
		1.0f,  
		false
	);

	LastAttackTime = GetWorld()->GetTimeSeconds();
}

void ARangedEnemy::PlayAttackAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && RangedAttackMontage)
	{
		GetCharacterMovement()->DisableMovement();
		AnimInstance->Montage_Play(RangedAttackMontage);

		float MontageDuration = RangedAttackMontage->GetPlayLength();
		GetWorldTimerManager().SetTimer(TimerAttack, [this]() {
			if (GetCharacterMovement())
			{
				GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
			bIsCharging = false;
			},
			MontageDuration,
			false
		);
	}
	else
	{
		FireProjectile();
		bIsCharging = false;
	}
}

void ARangedEnemy::ShowChargingEffect()
{
	if (!TargetActor)
		return;

	float ChargeTime = GetWorld()->GetTimeSeconds() - ChargeStartTime;
	float ChargePercent = FMath::Clamp(ChargeTime / 2.0f, 0.0f, 2.0f);  

	FVector MuzzleLocation;
	if (GetMesh() && GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		MuzzleLocation = GetMesh()->GetSocketLocation(MuzzleSocketName);
	}
	else
	{
		MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
	}

	FColor ChargeColor = FLinearColor::LerpUsingHSV(FLinearColor::Yellow,FLinearColor::Red,ChargePercent).ToFColor(true);

	float SphereSize = 10.0f + (40.0f * ChargePercent);
	DrawDebugSphere(GetWorld(),MuzzleLocation,SphereSize,12,ChargeColor,false,0.1f);

	FVector TargetLocation = TargetActor->GetActorLocation();
	DrawDebugLine(GetWorld(),MuzzleLocation,TargetLocation,ChargeColor,false,0.1f,0,2.0f + (3.0f * ChargePercent));
}

bool ARangedEnemy::CanAttack() const
{
	if (bIsStunned || Health <= 0.0f || !TargetActor)
	{
		return false;
	}
		
	float TimeSinceLastAttack = GetWorld()->GetTimeSeconds() - LastAttackTime;
	return TimeSinceLastAttack >= AttackCooldown && IsTargetInRange() && HasLineOfSight();
}

void ARangedEnemy::FireProjectile()
{
	if (!TargetActor)
		return;

	FVector MuzzleLocation;

	if (GetMesh() && GetMesh()->DoesSocketExist(MuzzleSocketName))
	{
		MuzzleLocation = GetMesh()->GetSocketLocation(MuzzleSocketName);
	}
	else
	{
		MuzzleLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
	}

	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector Direction = (TargetLocation - MuzzleLocation).GetSafeNormal();

	DrawDebugLine(GetWorld(),MuzzleLocation,TargetLocation,FColor::Red,false,2.0f, 0,5.0f);

	DrawDebugSphere(GetWorld(),TargetLocation,25.0f,12,FColor::Orange,false,2.0f);

	UGameplayStatics::ApplyDamage(TargetActor,ProjectileDamage,EnemyAIController,this,UDamageType::StaticClass());
}

bool ARangedEnemy::IsTargetInRange() const
{
	if (!TargetActor)
	{
		return false;
	}
		

	float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
	return Distance >= MinAttackRange && Distance <= AttackRange;
}

void ARangedEnemy::FindTarget()
{
	if (!TargetActor)
	{
		TargetActor = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
}

bool ARangedEnemy::HasLineOfSight() const
{
	if (!TargetActor)
	{
		return false;
	}

	FVector Start = GetActorLocation();
	FVector End = TargetActor->GetActorLocation();

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECC_Visibility,QueryParams);

	return !HitResult.bBlockingHit || HitResult.GetActor() == TargetActor;
}
