// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project/WukongGame/Public/Enemy/AI/PatrolPath.h"
#include "Engine/DataAsset.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MeleeHitInterface.h"
#include "Enemy.generated.h"

class AEnemyAIController;

UCLASS()
class WUKONG_API AEnemy : public ACharacter, public IMeleeHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UBehaviorTree* GetBehaviorTree() const
	{
		return Tree;
	}

	APatrolPath* GetPatrolPath() const;

	virtual void Tick(float DeltaTime) override;

	virtual void MeleeHit_Implementation(FHitResult HitResult) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE float GetBaseDamage() const { return BaseDamage; }
	FORCEINLINE bool IsStunned() const { return bIsStunned; }
	FORCEINLINE bool CanBeStunned() const { return bCanBeStunned; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))

	UBehaviorTree* Tree;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	AEnemyAIController* EnemyAIController;

	UFUNCTION(BlueprintImplementableEvent)
	void EnemyDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void PlayHitReaction(const FVector& HitDirection);

	void StartStun(float Duration);
	void EndStun();

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsStunned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bCanBeStunned = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float StunDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float StunCooldown = 20.0f; 

	FTimerHandle StunTimerHandle;
	FTimerHandle StunCooldownTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionFrontMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionBackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionLeftMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionRightMontage;

	FVector GetHitDirection(AActor* DamageCauser);
	void PlayDirectionalHitReaction(const FVector& HitDirection);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName EnemyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	APatrolPath* PatrolPath;

};
