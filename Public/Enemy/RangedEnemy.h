// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project/WukongGame/Public/Enemy/Enemy.h"
#include "RangedEnemy.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API ARangedEnemy : public AEnemy
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    ARangedEnemy();
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MinAttackRange = 300.0f; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackCooldown = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ProjectileDamage = 20.0f;

   /* UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<class AProjectile> ProjectileClass;*/

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FName MuzzleSocketName = "MuzzleSocket";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    UAnimMontage* RangedAttackMontage;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void RangedAttack();


    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool CanAttack() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void FireProjectile();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool IsTargetInRange() const;

protected:
    AActor* TargetActor;
    float LastAttackTime;

    FTimerHandle TimerAttack;

    void FindTarget();
    bool HasLineOfSight() const;

    bool bIsCharging = false;
    float ChargeStartTime = 2.0f;
    void ShowChargingEffect();

    FTimerHandle ChargeTimer;

    void PlayAttackAnimation();

};
