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
class AEnemyAttackTokenManager;

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

	bool RequestAttackToken();
	void ReleaseAttackToken();
	bool HasAttackToken() const { return bHasAttackToken; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	AEnemyAttackTokenManager* TokenManager;
	bool bHasAttackToken = false;
	
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float TokenRetryDelay = 1.0f;
};
