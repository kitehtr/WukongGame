// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "WukongCharacterInterface.h"
#include "Project/WukongGame/Public/UI/UWB_ComboWidget.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
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

	UFUNCTION(Exec, Category = "Debug")
	void ResetCombatState();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 EnemiesDefeated;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	float GameStartTime;

	UFUNCTION(BlueprintCallable, Category = "Game Stats")
	void StartGameTimer();

	UFUNCTION(BlueprintCallable, Category = "Game Stats")
	void AddEnemyDefeated();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Movement and Camera
	void CustomJump();
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
	void PlayHitReaction(AActor* DamageCauser);
	void EndHitReaction();
	void OnHitReactionEnded(UAnimMontage* Montage, bool bInterrupted);

	FVector GetHitDirectionFromAttacker(AActor* DamageCauser);
	void PlayDirectionalHitReaction(const FVector& HitDirection);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionFrontMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionBackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionLeftMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionRightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HitReactChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsInHitReact = false;

	void PlayAnimMontage(UAnimMontage* MontageToPlay, FName SectionName = "Default");
	void OnDodgeEnded(UAnimMontage* Montage, bool bInterrupted);
	void EnableWalk();
	FName GetAttackSectionName(int32 SectionCount);
	FName GetHeavyAttackSectionName(int32 HeavySectionCount);

	void ForceAttackEnd();
	bool bCanAttack = true;
	FTimerHandle AttackCooldownTimer;

	void BufferAttackInput();
	void ClearAttackDelay();

	void MainAttack();
	void ExecuteRegularAttack();
	void ExecuteLockOnAttack();


	void FindAttackTarget();
	void UpdateNearbyEnemies();

	void MoveToTargetToAttack();
	void ExecuteAttack();

	void AirAttack();
	void HeavyAttack();
	void AOEDamage();
	void OnAttackEnded(UAnimMontage* Montage, bool bInterrupted);
	void EnableMovement();

	FTimerHandle HeavyAttackInputBuffer;
	bool bHeavyAttackDelay = false;
	void ClearHeavyAttackDelay();
	void BufferHeavyAttackInput();

	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void DeathOfPlayer();

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool CanDodge = true;

	//scores
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	int32 ComboCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	float ComboScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	float ComboTimeLimit = 3.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	float ComboMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	float BaseAttackScore = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	float HeavyAttackScoreMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	float AirAttackScoreMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TArray<FString> ComboRankNames = { "D", "C", "B", "A", "S"};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TArray<int32> ComboRankThresholds = { 0, 50, 100, 200, 400, 800, 1500 };

	FTimerHandle ComboDecayTimer;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void AddComboHit(EAttackType AttackType);

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetComboScore();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void UpdateComboMultiplier();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	FString GetComboRank() const;

	UFUNCTION(BlueprintCallable, Category = "Combo")
	int32 GetComboCount() const { return ComboCount; }

	UFUNCTION(BlueprintCallable, Category = "Combo")
	float GetComboScore() const { return ComboScore; }

	UFUNCTION(BlueprintCallable, Category = "Combo")
	float GetComboMultiplier() const { return ComboMultiplier; }

	void StartComboDecayTimer();


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

	FTimerHandle AttackFailsafeHandle;
	FTimerHandle MovementRecoveryHandle;

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
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	int CurrentJumpCounter = 0;


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

	bool bCanHeavyAttack;
	FTimerHandle HeavyAttackCooldownTimer;
	float HeavyAttackCooldownTime = 3.5f;

	//resetting combos
	FTimerHandle LightComboResetTimer;
	FTimerHandle HeavyComboResetTimer;


	void ResetCombo();
	void ResetHeavyCombo();

	FTimerHandle HitReactTimer;

	TSet<AActor*> AlreadyHitActors;

	EAttackType CurrentAttackType;

	//Enemy Vision 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionStimuliSourceComponent* StimulusSource;

	void SetupStimulusSource();

	//UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUWB_ComboWidget> ComboWidgetClass;

	UPROPERTY()
	class UUWB_ComboWidget* ComboWidgetInstance;

	void CreateComboWidget();
};
