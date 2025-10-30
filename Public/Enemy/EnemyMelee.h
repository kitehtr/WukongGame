// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Enemy/Enemy.h"
#include "Components/BoxComponent.h"
#include "EnemyMelee.generated.h"

/**
 * 
 */
class UAnimMontage;

UCLASS()
class WUKONG_API AEnemyMelee : public AEnemy
{
	GENERATED_BODY()
	
public:
	AEnemyMelee();

	virtual void ActivateRightWeapon();
	virtual void DeactivateRightWeapon();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* MeleeAttackMontage;
	UFUNCTION(BlueprintCallable)
	void MainMeleeAttack();

protected:
	virtual void BeginPlay() override;

	void MeleeAttack();

	void ResetMovementWalking();

	bool bIsDead = false;

	FName GetAttackSectionName(int32 SectionCount);

	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;

	FTimerHandle TimerAttack;

	

	TSet<AActor*> AlreadyHitActors;
	
};
