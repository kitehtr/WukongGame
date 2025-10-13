// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Enemy/Enemy.h"
#include "../../Public/Enemy/EnemyAIController.h"

// Sets default values
AEnemy::AEnemy() :
	Health(100.0f),
	MaxHealth(100.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	EnemyAIController = Cast <AEnemyAIController> (GetController());
}


// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::MeleeHit_Implementation(FHitResult HitResult)
{

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.0f)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Enemy Died"));
		Health = 0.0f;

		EnemyDeath();
	}
	else 
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

