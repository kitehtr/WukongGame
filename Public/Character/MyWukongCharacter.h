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

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None,
	BasicAttack,
	HeavyAttack,
	AirAttack
};

UCLASS()
class WUKONG_API AMyWukongCharacter : public ACharacter, public IWukongCharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyWukongCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	virtual void ActivateRightWeapon();
	virtual void DeactivateRightWeapon();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsDoubleJumping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsTripleJumping;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Movement and Camera
	virtual void OnJumped_Implementation() override;
	virtual void Landed(const FHitResult& Hit) override;
	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnRate(float Rate);
	void LookUpRate(float Rate);

	void Running();
	void StopRunning();

	/*void Recall();*/

	//Montages & animations handling
	UFUNCTION()
	void HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& a_pBranchingPayload);

	void PlayAnimMontage(UAnimMontage* MontageToPlay, FName SectionName = "Default");
	void OnDodgeEnded(UAnimMontage* Montage, bool bInterrupted);
	void EnableWalk();
	FName GetAttackSectionName(int32 SectionCount);
	FName GetHeavyAttackSectionName(int32 HeavySectionCount);

	bool bCanAttack = true;
	FTimerHandle AttackCooldownTimer;

	void BufferAttackInput();
	void ClearAttackDelay();

	void MainAttack();

	void FindAttackTarget();
	void UpdateNearbyEnemies();

	void MoveToTargetToAttack();
	void ExecuteAttack();

	void AirAttack();
	void HeavyAttack();
	void AOEDamage();
	void OnAttackEnded(UAnimMontage* Montage, bool bInterrupted);
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

	float DelayTimeForAttack = 0.7f;

	int32 ComboCounter;
	int32 HeavyAttackComboCounter;

	bool bAttackDelay = false;
	FTimerHandle AttackInputBuffer;

	AActor* CurrentTarget = nullptr;
	float MaxAttackRange = 2000.0f; 
	TArray<AActor*> NearbyEnemies; 

	bool bIsMovingToTarget = false;
	FTimerHandle MoveToTargetTimer;

	bool bRotatingToTarget = false;
	FRotator TargetSmoothRotation;
	float RotationSpeedToTarget = 5.0f;

	FTimerHandle RotationCompleteTimer;
	FTimerHandle RotationCheckTimer;

	FTimerHandle TimerMovementWalking;

	//Montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RecallMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MainAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AirAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DoubleJumpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* TripleJumpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage;

	void Dodge();
	bool bIsDodging = false;
	bool CanDodge = true;


	//Damage Calculations and Attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat",meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsHeavyAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsAirAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HeavyAttackDamage;

	//resetting combos
	FTimerHandle LightComboResetTimer;
	FTimerHandle HeavyComboResetTimer;


	void ResetCombo();
	void ResetHeavyCombo();

	TSet<AActor*> AlreadyHitActors;

	EAttackType CurrentAttackType;

};
