// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	AEnemyAIController* EnemyAIController;

	UFUNCTION(BlueprintImplementableEvent)
	void EnemyDeath();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess="true"))
	UBehaviorTree* Tree;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void MeleeHit_Implementation(FHitResult HitResult) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//Getter
	FORCEINLINE float GetBaseDamage() const { return BaseDamage; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	FName EnemyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	float MaxHealth;

};
