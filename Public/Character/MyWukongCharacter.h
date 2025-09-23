// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "WukongCharacterInterface.h"
#include "MyWukongCharacter.generated.h"

//Declarations
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;

UCLASS()
class WUKONG_API AMyWukongCharacter : public ACharacter, public IWukongCharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyWukongCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void ActivateRightWeapon();
	virtual void DeactivateRightWeapon();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnRate(float Rate);
	void LookUpRate(float Rate);

	void Running();
	void StopRunning();

	void Recall();

	void PlayAnimMontage(UAnimMontage* MontageToPlay, FName SectionName = "Default");

	void EnableWalk();

	FName GetAttackSectionName(int32 SectionCount);

	void MainAttack();

	void EnableMovement();

	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void DeathOfPlayer();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta=(AllowPrivateAccess="true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta=(AllowPrivateAccess="true"))
	UCameraComponent* FollowCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float DefaultTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float DefaultLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RunSpeed;

	float DelayTimeForAttack = 0.2f;

	int32 ComboCounter;

	FTimerHandle TimerMovementWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RecallMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MainAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	bool bIsAttacking;

	void ResetCombo();

	TSet<AActor*> AlreadyHitActors;
};
