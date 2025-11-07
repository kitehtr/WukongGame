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
	AMyWukongCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//Weapon Interface
	virtual void ActivateRightWeapon();
	virtual void DeactivateRightWeapon();

	//Health
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	//Game Stats and Progression
	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int32 EnemiesDefeated = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	int CoinBalance = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Game Stats")
	bool bIsGamePaused;

	UPROPERTY(BlueprintReadOnly, Category = "Game Stats")
	float CurrentGameTime;

	UFUNCTION(BlueprintCallable, Category = "Game Stats")
	void AddEnemyDefeated();

	UFUNCTION(BlueprintCallable, Category = "Game Stats")
	void AddCoin(int32 CoinValue);

	//Achievements
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievements")
	TSubclassOf<UUserWidget> AchievementWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Achievements")
	TSet<FName> UnlockedAchievements;

	UFUNCTION(BlueprintCallable, Category = "Achievements")
	void UnlockAchievement(FName AchievementName);

	UFUNCTION(BlueprintCallable, Category = "Achievements")
	void CheckAchievements();

	//Combos
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	int32 ComboCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	float ComboScore = 0.0f;

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
	TArray<FString> ComboRankNames = { "D", "C", "B", "A", "S" };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TArray<int32> ComboRankThresholds = { 0, 50, 100, 200, 400, 800, 1500 };

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

	//Debugs and Jump Conditions
	UFUNCTION(Exec, Category = "Debug")
	void ResetCombatState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsDoubleJumping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsTripleJumping;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool CanDodge = true;

protected:
	//Overrides
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;

	//Movement and Camera
	void CustomJump();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRate(float Rate);
	void LookUpRate(float Rate);
	void Running();
	void StopRunning();
	void Dodge();
	/*void Recall();*/

	//Combo System
	void MainAttack();
	void HeavyAttack();
	void AirAttack();
	void AOEDamage();
	void ExecuteRegularAttack();
	void ExecuteLockOnAttack();
	void FindAttackTarget();
	void UpdateNearbyEnemies();
	void MoveToTargetToAttack();
	void ExecuteAttack();
	void ForceAttackEnd();

	//Montages & animations handling
	void PlayAnimMontage(UAnimMontage* MontageToPlay, FName SectionName = "Default");

	UFUNCTION()
	void HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& a_pBranchingPayload);

	//Hit Reactions
	void PlayHitReaction(AActor* DamageCauser);
	void EndHitReaction();
	void OnHitReactionEnded(UAnimMontage* Montage, bool bInterrupted);
	FVector GetHitDirectionFromAttacker(AActor* DamageCauser);
	void PlayDirectionalHitReaction(const FVector& HitDirection);

	//Animation Assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionFrontMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionBackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionLeftMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animation")
	UAnimMontage* HitReactionRightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Combat")
	UAnimMontage* MainAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Combat")
	UAnimMontage* AirAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Combat")
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Movement")
	UAnimMontage* DoubleJumpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Movement")
	UAnimMontage* TripleJumpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Movement")
	UAnimMontage* DodgeMontage;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Combat")
	UAnimMontage* RecallMontage;*/

	//UI System
	void CreateComboWidget();
	void StartComboDecayTimer();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUWB_ComboWidget> ComboWidgetClass;

	//AI
	void SetupStimulusSource();

	//Combo Utilities
	FName GetAttackSectionName(int32 SectionCount);
	FName GetHeavyAttackSectionName(int32 HeavySectionCount);
	void ResetCombo();
	void ResetHeavyCombo();
	void EnableWalk();
	void EnableMovement();
	void OnDodgeEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnAttackEnded(UAnimMontage* Montage, bool bInterrupted);

	//Weapon System
	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void DeathOfPlayer();

	//Input Buffer
	void BufferAttackInput();
	void BufferHeavyAttackInput();
	void ClearAttackDelay();
	void ClearHeavyAttackDelay();
	float DelayTimeForAttack = ATTACK_COOLDOWN_TIME;
	
private:
	// Movement Constants
	static constexpr float DEFAULT_TURN_RATE = 45.0f;
	static constexpr float DEFAULT_LOOK_UP_RATE = 45.0f;
	static constexpr float WALK_SPEED = 750.0f;
	static constexpr float RUN_SPEED = 1500.0f;
	static constexpr float JUMP_Z_VELOCITY = 1000.0f;
	static constexpr float GRAVITY_SCALE = 1.25f;
	static constexpr float AIR_CONTROL = 0.25f;
	static constexpr int32 MAX_JUMP_COUNT = 3;

	// Combat Constants
	static constexpr float BASE_DAMAGE = 10.0f;
	static constexpr float HEAVY_ATTACK_DAMAGE = 25.0f;
	static constexpr float MAX_HEALTH = 100.0f;
	static constexpr float HEAVY_ATTACK_COOLDOWN = 1.0f;

	// Attack Ranges & Distances
	static constexpr float MAX_ATTACK_RANGE = 1500.0f;
	static constexpr float TARGET_ACCEPTANCE_DISTANCE = 150.0f;
	static constexpr float AOE_RADIUS = 350.0f;
	static constexpr float AOE_HALF_HEIGHT = 30.0f;

	// Force & Launch Values
	static constexpr float JUMP_LAUNCH_FORCE = 1000.0f;
	static constexpr float DODGE_GROUND_FORCE = 2000.0f;
	static constexpr float DODGE_AIR_FORCE = 3000.0f;
	static constexpr float AIR_ATTACK_DOWNWARD_FORCE = 5000.0f;

	// Timer Durations
	static constexpr float DODGE_FRICTION_RESET_DELAY = 0.3f;
	static constexpr float SAFETY_MOVEMENT_DELAY = 0.3f;
	static constexpr float AIR_ATTACK_INPUT_DELAY = 3.0f;
	static constexpr float ATTACK_COOLDOWN_TIME = 0.2f;
	static constexpr float COMBO_RESET_TIME = 4.0f;
	static constexpr float HEAVY_COMBO_RESET_TIME = 2.0f;
	static constexpr float INPUT_BUFFER_TIME = 0.2f;

	// Rotation & Physics
	static constexpr float ROTATION_SPEED_TO_TARGET = 50.0f;
	static constexpr float ROTATION_ANGLE_THRESHOLD = 1.0f;
	static constexpr float GROUND_FRICTION_NORMAL = 8.0f;
	static constexpr float GROUND_FRICTION_DODGE = 0.0f;

	//Component References
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta=(AllowPrivateAccess="true"))
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta=(AllowPrivateAccess="true"))
	UCameraComponent* FollowCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionStimuliSourceComponent* StimulusSource;
	
	//Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = WALK_SPEED;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RunSpeed = RUN_SPEED;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage = BASE_DAMAGE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HeavyAttackDamage = HEAVY_ATTACK_DAMAGE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float Health = MAX_HEALTH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = MAX_HEALTH;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float HitReactChance = 1.0f;

	//Runtime State
	bool bIsAttacking = false;
	bool bIsHeavyAttacking = false;
	bool bIsAirAttacking = false;
	bool bIsInHitReact = false;
	bool bIsDodging = false;
	bool bCanAttack = true;
	bool bCanHeavyAttack = true;
	bool bAttackDelay = false;
	bool bHeavyAttackDelay = false;

	//Movement States
	bool bIsMovingToTarget = false;
	bool bRotatingToTarget = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	int32 CurrentJumpCounter = 0;

	//Combat Tracking
	AActor* CurrentTarget = nullptr;
	TArray<AActor*> NearbyEnemies;
	TSet<AActor*> AlreadyHitActors;
	EAttackType CurrentAttackType = EAttackType::None;
	float MaxAttackRange = MAX_ATTACK_RANGE;

	//Combo System
	int32 ComboCounter = 0;
	int32 HeavyAttackComboCounter = 0;

	//Rotation & Movement
	float RotationSpeedToTarget = ROTATION_SPEED_TO_TARGET;
	FRotator TargetSmoothRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float DefaultTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float DefaultLookUpRate;

	//Timer Handles
	float HeavyAttackCooldownTime = HEAVY_ATTACK_COOLDOWN;
	FTimerHandle AttackCooldownTimer;
	FTimerHandle HeavyAttackCooldownTimer;
	FTimerHandle LightComboResetTimer;
	FTimerHandle HeavyComboResetTimer;
	FTimerHandle AttackInputBuffer;
	FTimerHandle HeavyAttackInputBuffer;
	FTimerHandle AttackFailsafeHandle;
	FTimerHandle MovementRecoveryHandle;
	FTimerHandle MoveToTargetTimer;
	FTimerHandle ComboDecayTimer;
	FTimerHandle RotationCompleteTimer;
	FTimerHandle RotationCheckTimer;
	FTimerHandle TimerMovementWalking;
	FTimerHandle HitReactTimer;

	//UI
	UPROPERTY()
	class UUWB_ComboWidget* ComboWidgetInstance;

};
