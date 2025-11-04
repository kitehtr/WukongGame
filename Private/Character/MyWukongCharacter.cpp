#include "../../Public/Character/MyWukongCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/EngineTypes.h"  
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h" 
#include "Components/ShapeComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Public/Enemy/MeleeHitInterface.h"
#include "../../Public/Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Project/WukongGame/Public/UI/UWB_ComboWidget.h"
#include "Blueprint/UserWidget.h"

AMyWukongCharacter::AMyWukongCharacter() :
	DefaultTurnRate(45.0f),
	DefaultLookUpRate(45.0f),
	WalkSpeed(750.0f),
	RunSpeed(1500.0f),
	BaseDamage(10.0f),
	Health(100.0f),
	MaxHealth(100.0f),
	HeavyAttackCooldownTime(1.0f),
	bCanHeavyAttack(true),
	ComboCount(0),
	ComboScore(0.0f),
	ComboMultiplier(1.0f)

{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	FollowCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	FollowCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	FollowCameraComponent->bUsePawnControlRotation = false;

	SetupStimulusSource();

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 1000.0f;
	GetCharacterMovement()->GravityScale = 1.25f;
	GetCharacterMovement()->AirControl = 0.25f;
	JumpMaxCount = 3;

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));
}

void AMyWukongCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentGameTime = GetWorld()->GetTimeSeconds();

	if (bRotatingToTarget)
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetSmoothRotation, DeltaTime, RotationSpeedToTarget);

		NewRotation.Pitch = CurrentRotation.Pitch;
		NewRotation.Roll = CurrentRotation.Roll;
		SetActorRotation(NewRotation);

		if (Controller)
		{
			FRotator ControllerRot = Controller->GetControlRotation();
			ControllerRot.Yaw = NewRotation.Yaw;
			Controller->SetControlRotation(ControllerRot);
		}

		float AngleDifference = FMath::Abs(CurrentRotation.Yaw - TargetSmoothRotation.Yaw);
		if (AngleDifference < 1.0f) 
		{
			bRotatingToTarget = false;
			SetActorRotation(TargetSmoothRotation);
		}

	}
}

void AMyWukongCharacter::HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& a_pBranchingPayload)
{
	if (NotifyName.ToString() == "Dodge")
	{
		bIsDodging = false;
	}
}

void AMyWukongCharacter::AddEnemyDefeated()
{
	EnemiesDefeated++;
}

void AMyWukongCharacter::BeginPlay()
{
	Super::BeginPlay();

	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AMyWukongCharacter::OnRightWeaponOverlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
	if (pAnimInst != nullptr)
	{
		pAnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &AMyWukongCharacter::HandleOnMontageNotifyBegin);
	}

	if (StimulusSource)
	{
		StimulusSource->RegisterForSense(UAISense_Sight::StaticClass());
		StimulusSource->RegisterWithPerceptionSystem();
	}

	CreateComboWidget();
	
}

void AMyWukongCharacter::CreateComboWidget()
{
	if (ComboWidgetClass)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			ComboWidgetInstance = CreateWidget<UUWB_ComboWidget>(PC, ComboWidgetClass);
			if (ComboWidgetInstance)
			{
				ComboWidgetInstance->AddToViewport();
				ComboWidgetInstance->HideComboWidget();
			}
		}
	}
}

void AMyWukongCharacter::CustomJump()
{

	if (CurrentJumpCounter >= 3)
	{
		return;
	}

	LaunchCharacter(FVector(0, 0, 1000.0f), false, true);

	if (CurrentJumpCounter == 0)
	{
		CurrentJumpCounter++;
	}
	else if (CurrentJumpCounter == 1)
	{
		CanDodge = true;
		bIsDoubleJumping = true;
		CurrentJumpCounter++;
	}
	else if (CurrentJumpCounter == 2)
	{
		CanDodge = true;
		bIsTripleJumping = true;
		CurrentJumpCounter++;
	}
}

void AMyWukongCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	GetCharacterMovement()->GravityScale = 1.25f;
	CurrentJumpCounter = 0;
	CanDodge = true;
	bIsDoubleJumping = false;
	bIsTripleJumping = false;
	bCanAttack = true;
	bIsAttacking = false;
	bIsHeavyAttacking = false;
	bCanHeavyAttack = true;

	if (bIsAirAttacking)
	{
		AOEDamage();
		bIsAirAttacking = false;
	}
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		float DelayTime = 1.0f;

		FTimerHandle InputTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(InputTimerHandle, [PC]() {PC->SetIgnoreMoveInput(false);PC->SetIgnoreLookInput(false);}, DelayTime, false);
	}
}

void AMyWukongCharacter::MoveForward(float Value)
{
	if (bIsAttacking || bIsHeavyAttacking || bIsInHitReact)
	{
		return;
	}

	if ((Controller != nullptr) && (Value != 0.0f) && !bIsAttacking && !bIsHeavyAttacking)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AMyWukongCharacter::MoveRight(float Value)
{
	if (bIsAttacking || bIsHeavyAttacking || bIsInHitReact)
	{
		return;
	}		

	if ((Controller != nullptr) && (Value != 0.0f) && !bIsAttacking && !bIsHeavyAttacking)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AMyWukongCharacter::TurnRate(float Rate)
{
	AddControllerYawInput(Rate * DefaultTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyWukongCharacter::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * DefaultLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyWukongCharacter::Running()
{
	if (GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void AMyWukongCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMyWukongCharacter::Dodge()
{
	if (bIsInHitReact)
	{
		return;
	}

	if (bIsAttacking || bIsHeavyAttacking)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && MainAttackMontage)
		{
			AnimInstance->Montage_Stop(0.1f, MainAttackMontage);
			OnAttackEnded(MainAttackMontage, true); 
			CanDodge = true;
		}

		if (AnimInstance && HeavyAttackMontage)
		{
			AnimInstance->Montage_Stop(0.1f, HeavyAttackMontage);
			OnAttackEnded(HeavyAttackMontage, true);
			CanDodge = true;
		}

		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	if (!bIsDodging && CanDodge)
	{
		UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
		if (pAnimInst != nullptr && DodgeMontage)
		{
			bIsDodging = true;
			CanDodge = false;

			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &AMyWukongCharacter::OnDodgeEnded);
			pAnimInst->Montage_SetEndDelegate(MontageEndedDelegate, DodgeMontage);

			if (pAnimInst->Montage_Play(DodgeMontage))
			{
				FVector DodgeDirection = GetLastMovementInputVector();
				if (DodgeDirection.IsNearlyZero())
				{
					DodgeDirection = GetActorForwardVector();
				}
				DodgeDirection.Normalize();

				if (GetCharacterMovement()->IsMovingOnGround())
				{
					GetCharacterMovement()->Velocity = FVector::ZeroVector; 
					GetCharacterMovement()->GroundFriction = 0.0f; 
					LaunchCharacter(DodgeDirection * 2000.0f, false, true); 
					FTimerHandle FrictionTimer;
					GetWorld()->GetTimerManager().SetTimer(FrictionTimer, [this]() {
						GetCharacterMovement()->GroundFriction = 8.0f; 
						}, 0.3f, false);
				}
				else
				{
					LaunchCharacter(DodgeDirection * 3000.0f, true, true);
				}
			}
			else
			{
				OnDodgeEnded(nullptr, true);
			}
		}
		else
		{
			bIsDodging = false;
			CanDodge = true;
		}
	}
}

void AMyWukongCharacter::OnDodgeEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsDodging = false;
	CanDodge = true;
	if (GetCharacterMovement()->MovementMode != MOVE_Walking)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

//void AMyWukongCharacter::Recall()
//{
//	PlayAnimMontage(RecallMontage, FName("Recall"));
//}

void AMyWukongCharacter::PlayAnimMontage(UAnimMontage* MontageToPlay, FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MontageToPlay)
	{
		if (!AnimInstance->Montage_IsPlaying(MontageToPlay))
		{
			GetCharacterMovement()->DisableMovement();

			int32 const SectionIndex = MontageToPlay->GetSectionIndex(SectionName);
			int32 const SectionLength = MontageToPlay->GetSectionLength(SectionIndex);

			AnimInstance->Montage_Play(MontageToPlay);
			AnimInstance->Montage_JumpToSection(SectionName);

			GetWorldTimerManager().SetTimer(TimerMovementWalking, this, &AMyWukongCharacter::EnableWalk, SectionLength);
		}
	}
}

void AMyWukongCharacter::EnableWalk()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AMyWukongCharacter::MainAttack()
{
	if (bIsDodging || bIsHeavyAttacking || bIsInHitReact || !bCanAttack)
	{
		if (!bAttackDelay)
		{
			bAttackDelay = true;
			GetWorld()->GetTimerManager().SetTimer(AttackInputBuffer, this, &AMyWukongCharacter::ClearAttackDelay, 0.2f, false);
		}
		return;
	}

	if (bIsAttacking)
	{
		return;
	}

	if (bAttackDelay)
	{
		bAttackDelay = false;
		GetWorld()->GetTimerManager().ClearTimer(AttackInputBuffer);
	}

	GetWorld()->GetTimerManager().ClearTimer(AttackCooldownTimer);
	GetWorld()->GetTimerManager().ClearTimer(LightComboResetTimer);
	GetWorld()->GetTimerManager().ClearTimer(AttackFailsafeHandle);

	if (!bIsAttacking && !bIsMovingToTarget)
	{
		FindAttackTarget();
	}

	if (CurrentTarget && !bIsMovingToTarget)
	{
		MoveToTargetToAttack();
		return;
	}

	if (GetCharacterMovement()->IsFalling())
	{
		AirAttack();
		return;
	}

	ExecuteRegularAttack();

}

void AMyWukongCharacter::ExecuteRegularAttack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MainAttackMontage)
	{
		bIsAttacking = true;
		bCanAttack = false;
		AlreadyHitActors.Empty();
		CurrentAttackType = EAttackType::BasicAttack;
		FName const SectionName = GetAttackSectionName(MainAttackMontage->CompositeSections.Num());

		if (AnimInstance->Montage_IsPlaying(nullptr))
		{
			AnimInstance->Montage_Stop(0.1f);
		}

		FOnMontageEnded AttackEndedDelegate;
		AttackEndedDelegate.BindUObject(this, &AMyWukongCharacter::OnAttackEnded);
		AnimInstance->Montage_SetEndDelegate(AttackEndedDelegate, MainAttackMontage);

		if (!GetCharacterMovement()->IsFalling())
		{
			GetCharacterMovement()->DisableMovement();

			FTimerHandle SafetyMoveTimer;
			GetWorld()->GetTimerManager().SetTimer(SafetyMoveTimer, [this]() {
				if (GetCharacterMovement()->MovementMode != MOVE_Walking && !GetCharacterMovement()->IsFalling())
				{
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				}
				}, 0.3f, false); 
		}

		float PlayTime = AnimInstance->Montage_Play(MainAttackMontage);
		AnimInstance->Montage_JumpToSection(SectionName, MainAttackMontage);

		if (PlayTime > 0.0f)
		{
			float AttackDuration = PlayTime * 0.2f; 
			GetWorld()->GetTimerManager().SetTimer(AttackFailsafeHandle, this, &AMyWukongCharacter::ForceAttackEnd, AttackDuration, false);

			GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimer, [this]() {
				bCanAttack = true;
				}, 0.2f, false);
		}
		else
		{
			ForceAttackEnd();
		}

		GetWorld()->GetTimerManager().SetTimer(LightComboResetTimer, this, &AMyWukongCharacter::ResetCombo, 4.0f, false);
	}
	else
	{
		ForceAttackEnd();
	}
}

void AMyWukongCharacter::ForceAttackEnd()
{
	bool bHasBufferedHeavyAttack = GetWorld()->GetTimerManager().IsTimerActive(HeavyAttackInputBuffer) && bHeavyAttackDelay;

	GetWorld()->GetTimerManager().ClearTimer(AttackCooldownTimer);
	GetWorld()->GetTimerManager().ClearTimer(AttackFailsafeHandle);
	GetWorld()->GetTimerManager().ClearTimer(LightComboResetTimer);
	GetWorld()->GetTimerManager().ClearTimer(HeavyComboResetTimer);
	GetWorld()->GetTimerManager().ClearTimer(MoveToTargetTimer);

	if (!bHasBufferedHeavyAttack)
	{
		bIsHeavyAttacking = false;
		bCanHeavyAttack = true;
	}

	bIsAttacking = false;
	bIsAirAttacking = false;
	bCanAttack = true;
	CanDodge = true;
	bIsMovingToTarget = false;

	/*UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (!bHasBufferedHeavyAttack)
		{
			if (MainAttackMontage && AnimInstance->Montage_IsPlaying(MainAttackMontage))
			{
				AnimInstance->Montage_Stop(0.1f, MainAttackMontage);
			}
			if (HeavyAttackMontage && AnimInstance->Montage_IsPlaying(HeavyAttackMontage))
			{
				AnimInstance->Montage_Stop(0.1f, HeavyAttackMontage);
			}
		}
		else
		{
			if (MainAttackMontage && AnimInstance->Montage_IsPlaying(MainAttackMontage))
			{
				AnimInstance->Montage_Stop(0.1f, MainAttackMontage);
			}
		}

		if (AirAttackMontage && AnimInstance->Montage_IsPlaying(AirAttackMontage))
		{
			AnimInstance->Montage_Stop(0.1f, AirAttackMontage);
		}
	}*/

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement)
	{
		if (Movement->MovementMode != MOVE_Walking && !Movement->IsFalling())
		{
			Movement->SetMovementMode(MOVE_Walking);
		}
	}

	DeactivateRightWeapon();
}

void AMyWukongCharacter::OnAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != MainAttackMontage && Montage != HeavyAttackMontage && Montage != AirAttackMontage)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(AttackFailsafeHandle);
	ForceAttackEnd();

	if (bHeavyAttackDelay)
	{
		GetWorld()->GetTimerManager().SetTimer(HeavyAttackInputBuffer, this, &AMyWukongCharacter::BufferHeavyAttackInput, 1.3f, false);
	}

	if (bAttackDelay)
	{
		GetWorld()->GetTimerManager().SetTimer(AttackInputBuffer, this, &AMyWukongCharacter::BufferAttackInput, 0.1f, false);
	}
}

void AMyWukongCharacter::BufferAttackInput()
{
	if (bAttackDelay && !bIsAttacking && bCanAttack && !bIsDodging && !bIsHeavyAttacking)
	{
		bAttackDelay = false;
		GetWorld()->GetTimerManager().ClearTimer(AttackInputBuffer);
		MainAttack();
	}
}

void AMyWukongCharacter::BufferHeavyAttackInput()
{
	if (bHeavyAttackDelay && !bIsHeavyAttacking && bCanHeavyAttack && !bIsDodging && !bIsAttacking)
	{
		bHeavyAttackDelay = false;
		GetWorld()->GetTimerManager().ClearTimer(HeavyAttackInputBuffer);
		GetWorld()->GetTimerManager().SetTimer(HeavyAttackInputBuffer,[this](){HeavyAttack();},1.0f, false);
	}
}

void AMyWukongCharacter::MoveToTargetToAttack()
{
	if (!CurrentTarget || bIsMovingToTarget)
	{
		ExecuteRegularAttack();
		return;
	}

	bIsMovingToTarget = true;
	bIsAttacking = true;

	FVector TargetLocation = CurrentTarget->GetActorLocation();
	FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
	float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetLocation);

	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;

	TargetSmoothRotation = TargetRotation;
	bRotatingToTarget = true;

	if (DistanceToTarget < 150.0f)
	{
		ExecuteLockOnAttack(); 
		return;
	}

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	float LaunchSpeed = FMath::GetMappedRangeValueClamped(FVector2D(100.0f, 2000.0f), FVector2D(1200.0f, 3500.0f), DistanceToTarget);

	GetCharacterMovement()->Velocity = Direction * LaunchSpeed;
	GetCharacterMovement()->GroundFriction = 0.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 10.0f;

	float MoveTime = DistanceToTarget / LaunchSpeed;
	MoveTime = FMath::Clamp(MoveTime, 0.1f, 0.6f);

	GetWorld()->GetTimerManager().SetTimer(MoveToTargetTimer, this, &AMyWukongCharacter::ExecuteLockOnAttack, MoveTime, false);

	FTimerHandle SafetyTimer;
	GetWorld()->GetTimerManager().SetTimer(SafetyTimer, [this]() {
		if (bIsMovingToTarget)
		{
			ExecuteLockOnAttack(); 
		}
		}, 1.0f, false);
}

void AMyWukongCharacter::ExecuteLockOnAttack()
{
	bIsMovingToTarget = false;
	GetWorld()->GetTimerManager().ClearTimer(MoveToTargetTimer);

	AActor* AttackTarget = CurrentTarget;
	CurrentTarget = nullptr; 

	if (GetCharacterMovement()->IsFalling())
	{
		AirAttack();
		return;
	}

	if (AttackTarget)
	{
		FVector Direction = (AttackTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FRotator TargetRotation = Direction.Rotation();
		TargetRotation.Pitch = 0.0f;
		TargetRotation.Roll = 0.0f;

		TargetSmoothRotation = TargetRotation;
		bRotatingToTarget = true;
	}

	GetCharacterMovement()->GroundFriction = 8.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;

	ExecuteRegularAttack();
}

void AMyWukongCharacter::ExecuteAttack()
{
	bIsMovingToTarget = false;
	GetWorld()->GetTimerManager().ClearTimer(MoveToTargetTimer);
	GetWorld()->GetTimerManager().ClearTimer(AttackCooldownTimer);
	GetWorld()->GetTimerManager().ClearTimer(LightComboResetTimer);
	GetWorld()->GetTimerManager().ClearTimer(AttackFailsafeHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackFailsafeHandle);

	if (GetCharacterMovement()->IsFalling())
	{
		AirAttack();
		return;
	}

	if (CurrentTarget)
	{
		FVector Direction = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FRotator TargetRotation = Direction.Rotation();
		TargetRotation.Pitch = 0.0f;
		TargetRotation.Roll = 0.0f;

		TargetSmoothRotation = TargetRotation;
		bRotatingToTarget = true;
	}

	GetCharacterMovement()->GroundFriction = 8.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;

	FTimerHandle SafetyMovementTimer;
	GetWorld()->GetTimerManager().SetTimer(SafetyMovementTimer, [this]() {
		if (GetCharacterMovement()->MovementMode != MOVE_Walking)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		}, 0.5f, false);

	GetWorld()->GetTimerManager().ClearTimer(LightComboResetTimer);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MainAttackMontage)
	{
		AlreadyHitActors.Empty();
		bIsAttacking = true;
		bCanAttack = false;
		CurrentAttackType = EAttackType::BasicAttack;
		int32 const SectionCount = MainAttackMontage->CompositeSections.Num();

		FName const SectionName = GetAttackSectionName(SectionCount);

		if (!GetCharacterMovement()->IsFalling())
		{
			GetCharacterMovement()->DisableMovement();
		}

		FOnMontageEnded AttackEndedDelegate;
		AttackEndedDelegate.BindUObject(this, &AMyWukongCharacter::OnAttackEnded);
		AnimInstance->Montage_SetEndDelegate(AttackEndedDelegate, MainAttackMontage);

		AnimInstance->Montage_Play(MainAttackMontage);
		AnimInstance->Montage_JumpToSection(SectionName, MainAttackMontage);


		GetWorld()->GetTimerManager().SetTimer(LightComboResetTimer, this, &AMyWukongCharacter::ResetCombo, 4.0f, false);
		GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimer, [this]() {bCanAttack = true; }, 0.5f, false);
	}
	else
	{
		bIsAttacking = false;
		bCanAttack = true;
	}

	if (GetCharacterMovement()->MovementMode != MOVE_Walking)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AMyWukongCharacter::ClearAttackDelay()
{
	bAttackDelay = false;
	GetWorld()->GetTimerManager().ClearTimer(AttackInputBuffer);
}

void AMyWukongCharacter::FindAttackTarget()
{
	if (bIsAttacking || bIsMovingToTarget || bIsDodging || bIsInHitReact)
	{
		return;
	}

	UpdateNearbyEnemies();

	if (CurrentTarget && IsValid(CurrentTarget))
	{
		float CurrentDistance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
		if (CurrentDistance <= MaxAttackRange * 1.5f) 
		{
			return; 
		}
	}

	if (NearbyEnemies.Num() == 0)
	{
		CurrentTarget = nullptr;
		return;
	}

	FVector InputDirection = GetLastMovementInputVector();
	if (InputDirection.IsNearlyZero())
	{
		InputDirection = GetActorForwardVector();
	}

	AActor* BestTarget = nullptr;
	float BestScore = -1.0f;

	for (AActor* Enemy : NearbyEnemies)
	{
		if (!IsValid(Enemy))
		{
			continue;
		}

		AEnemy* EnemyPawn = Cast<AEnemy>(Enemy);
		if (!EnemyPawn)
		{
			continue;
		}

		FVector ToEnemy = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		float DotProduct = FVector::DotProduct(InputDirection, ToEnemy);
		float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());

		if (Distance > MaxAttackRange)
		{
			continue;
		}

		float DistanceScore = 1.0f - FMath::Clamp(Distance / MaxAttackRange, 0.0f, 1.0f);
		float DirectionScore = (DotProduct + 1.0f) * 0.5f;

		float Score = (DistanceScore * 0.7f) + (DirectionScore * 0.3f);

		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Enemy;
		}
	}

	CurrentTarget = BestTarget;
}

void AMyWukongCharacter::UpdateNearbyEnemies()
{
	NearbyEnemies.Empty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), FoundActors); 

	for (AActor* Actor : FoundActors)
	{
		float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
		if (Distance < MaxAttackRange * 2.0f) 
		{
			NearbyEnemies.Add(Actor);
		}
	}
}

void AMyWukongCharacter::AirAttack()
{
	bIsAttacking = false;
	bIsHeavyAttacking = false;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AirAttackMontage)
	{
		bIsAirAttacking = true;
		bCanAttack = false;

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->SetIgnoreMoveInput(true);
			PC->SetIgnoreLookInput(true);
		}

		FOnMontageEnded AttackEndedDelegate;
		AttackEndedDelegate.BindUObject(this, &AMyWukongCharacter::OnAttackEnded);
		AnimInstance->Montage_SetEndDelegate(AttackEndedDelegate, AirAttackMontage);

		AnimInstance->Montage_Stop(0.1f);

		AnimInstance->Montage_Play(AirAttackMontage);

		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		LaunchCharacter(FVector(0, 0, -5000.0f), true, true);

		FTimerHandle AirAttackInputTimer;
		GetWorld()->GetTimerManager().SetTimer(AirAttackInputTimer, [this, PC]() {
			if (PC && bIsAirAttacking)
			{
				PC->SetIgnoreMoveInput(false);
				PC->SetIgnoreLookInput(false);
			}
			}, 3.0f, false);
	}
	else
	{
		bIsAirAttacking = false;
		bCanAttack = true;
	}
}

void AMyWukongCharacter::AOEDamage()
{
	//aoe area
	float Radius = 350.0f;
	float HalfHeight = 30.0f;
	TArray<FOverlapResult> Overlaps;
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);

	//make sure not to damage self
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); 

	bool bHit = GetWorld()->OverlapMultiByChannel(Overlaps,GetActorLocation(),FQuat::Identity,ECC_Pawn,CollisionShape,QueryParams);

	if (bHit)
	{
		for (auto& Result : Overlaps)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (OverlappedActor)
			{	
				//checks for overlapped actors and applies the damage
				UGameplayStatics::ApplyDamage(OverlappedActor, 100.0f, GetController(), this, nullptr);
			}
		}
	}

	//debug visual
	/*DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 32, FColor::Red, false, 2.0f);*/
}

void AMyWukongCharacter::HeavyAttack()
{
	if (bIsHeavyAttacking)
	{
		return;
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(HeavyAttackCooldownTimer))
	{
		if (!bHeavyAttackDelay)
		{
			bHeavyAttackDelay = true;
			GetWorld()->GetTimerManager().SetTimer(HeavyAttackInputBuffer, this, &AMyWukongCharacter::ClearHeavyAttackDelay, 1.0f, false);
		}
		return;
	}

	if (bIsDodging || bIsAttacking || !bCanAttack || bIsHeavyAttacking || !CanDodge || !bCanHeavyAttack || bIsInHitReact)
	{
		if (!bHeavyAttackDelay)
		{
			bHeavyAttackDelay = true;
			GetWorld()->GetTimerManager().SetTimer(HeavyAttackInputBuffer, this, &AMyWukongCharacter::ClearHeavyAttackDelay, 1.0f, false);
		}
		return;
	}

	if (GetCharacterMovement()->IsFalling())
	{
		AirAttack();
		return;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(HeavyComboResetTimer);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HeavyAttackMontage)
	{
		if (AnimInstance->Montage_IsPlaying(nullptr))
		{
			return;
		}

		AlreadyHitActors.Empty();
		bIsHeavyAttacking = true;
		bCanHeavyAttack = false;
		CanDodge = false;
		CurrentAttackType = EAttackType::HeavyAttack;
		int32 const HeavySectionCount = HeavyAttackMontage->CompositeSections.Num();

		FName const HeavySectionName = GetHeavyAttackSectionName(HeavySectionCount);

		GetCharacterMovement()->DisableMovement();

		FOnMontageEnded AttackEndedDelegate;
		AttackEndedDelegate.BindUObject(this, &AMyWukongCharacter::OnAttackEnded);
		AnimInstance->Montage_SetEndDelegate(AttackEndedDelegate, HeavyAttackMontage);

		float const PlayRate = 1.0f;
		AnimInstance->Montage_Play(HeavyAttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(HeavySectionName, HeavyAttackMontage);
		
		GetWorld()->GetTimerManager().SetTimer(HeavyComboResetTimer, this, &AMyWukongCharacter::ResetHeavyCombo, 2.0f, false);
		GetWorld()->GetTimerManager().SetTimer(HeavyAttackCooldownTimer, [this]() {bCanHeavyAttack = true;}, HeavyAttackCooldownTime, false);

	}
}

void AMyWukongCharacter::ClearHeavyAttackDelay()
{
	bHeavyAttackDelay = false;
	GetWorld()->GetTimerManager().ClearTimer(HeavyAttackInputBuffer);
}

void AMyWukongCharacter::EnableMovement()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

FName AMyWukongCharacter::GetAttackSectionName(int32 SectionCount)
{
	TArray<FName> AttackSections = { "Attack1", "Attack2", "Attack3"};

	if (ComboCounter >= AttackSections.Num())
	{
		ComboCounter = 0; 
	}

	FName SectionName = AttackSections[ComboCounter % AttackSections.Num()];
	ComboCounter++;

	return SectionName;
}

FName AMyWukongCharacter::GetHeavyAttackSectionName(int32 HeavySectionCount)
{
	TArray<FName> HeavyAttackSections = { "HeavyAttack1", "HeavyAttack2"};

	if (HeavyAttackComboCounter >= HeavyAttackSections.Num())
	{
		HeavyAttackComboCounter = 0;
	}

	FName HeavySectionName = HeavyAttackSections[HeavyAttackComboCounter % HeavyAttackSections.Num()];
	HeavyAttackComboCounter++;

	RightWeaponCollision->SetRelativeScale3D(FVector(1.25f, 1.0f, 4.0f));

	if (HeavySectionName == "HeavyAttack1")
	{
		AOEDamage();
	}
	if (HeavySectionName == "HeavyAttack2")
	{
		//adjust weapon collision for extended staff
		RightWeaponCollision->SetRelativeScale3D(FVector(20.0f, 2.0f, 1.0f));
		RightWeaponCollision->SetRelativeRotation(FRotator(0.0f, -45.0f, 0.0f));
		RightWeaponCollision->SetRelativeLocation(FVector(-300.0f, 0.0f, 0.0f)); 

		RightWeaponCollision->UpdateOverlaps();
		/*GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Extended weapon scale applied"));*/
	}
	return HeavySectionName;
}

void AMyWukongCharacter::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (IsValid(SweepResult.GetActor()))
	{
		if (SweepResult.GetActor() == this)
		{
			return;
		}

		bool bIsEnemy = SweepResult.GetActor()->ActorHasTag("Enemy");
		if (bIsEnemy && AlreadyHitActors.Contains(SweepResult.GetActor()))
		{
			return;
		}

		IMeleeHitInterface* MeleeHitInterface = Cast<IMeleeHitInterface>(SweepResult.GetActor());
		if (MeleeHitInterface)
		{
			MeleeHitInterface->MeleeHit_Implementation(SweepResult);
		}

		float WeaponDamage = 0.0f;
		switch (CurrentAttackType)
		{
		case EAttackType::BasicAttack:
			WeaponDamage = BaseDamage;
			break;
		case EAttackType::HeavyAttack:
			WeaponDamage = HeavyAttackDamage;
			break;
		default:
			WeaponDamage = BaseDamage;
			break;
		}

		UGameplayStatics::ApplyDamage(SweepResult.GetActor(), WeaponDamage, GetController(), this, UDamageType::StaticClass());

		if (bIsEnemy)
		{
			AlreadyHitActors.Add(SweepResult.GetActor());
			AddComboHit(CurrentAttackType);
		}
	}
}
void AMyWukongCharacter::AddComboHit(EAttackType AttackType)
{
	ComboCount++;

	float ScoreToAdd = BaseAttackScore;

	switch (AttackType)
	{
	case EAttackType::BasicAttack:
		ScoreToAdd = BaseAttackScore;
		break;
	case EAttackType::HeavyAttack:
		ScoreToAdd = BaseAttackScore * HeavyAttackScoreMultiplier;
		break;
	case EAttackType::AirAttack:
		ScoreToAdd = BaseAttackScore * AirAttackScoreMultiplier;
		break;
	default:
		ScoreToAdd = BaseAttackScore;
		break;
	}

	UpdateComboMultiplier();
	ComboScore += ScoreToAdd * ComboMultiplier;

	FString ComboRank = GetComboRank();
	if (ComboWidgetInstance)
	{
		ComboWidgetInstance->UpdateComboDisplay(ComboCount, ComboScore, ComboRank, ComboMultiplier);
	}

	StartComboDecayTimer();
}

void AMyWukongCharacter::ResetComboScore()
{
	if (ComboCount > 0)
	{
		if (ComboWidgetInstance)
		{
			ComboWidgetInstance->HideComboWidget();
		}
	}

	ComboCount = 0;
	ComboScore = 0.0f;
	ComboMultiplier = 1.0f;

	GetWorld()->GetTimerManager().ClearTimer(ComboDecayTimer);
}

void AMyWukongCharacter::UpdateComboMultiplier()
{
	if (ComboCount >= 50)
	{
		ComboMultiplier = 3.0f;
	}
	else if (ComboCount >= 30)
	{
		ComboMultiplier = 2.5f;
	}
	else if (ComboCount >= 20)
	{
		ComboMultiplier = 2.0f;
	}
	else if (ComboCount >= 10)
	{
		ComboMultiplier = 1.5f;
	}
	else if (ComboCount >= 5)
	{
		ComboMultiplier = 1.2f;
	}
	else
	{
		ComboMultiplier = 1.0f;
	}
}

FString AMyWukongCharacter::GetComboRank() const
{
	for (int32 i = ComboRankThresholds.Num() - 1; i >= 0; i--)
	{
		if (ComboScore >= ComboRankThresholds[i])
		{
			if (i < ComboRankNames.Num())
			{
				return ComboRankNames[i];
			}
		}
	}

	return ComboRankNames.Num() > 0 ? ComboRankNames[0] : TEXT("D");
}

void AMyWukongCharacter::StartComboDecayTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(ComboDecayTimer);
	GetWorld()->GetTimerManager().SetTimer(ComboDecayTimer, this, &AMyWukongCharacter::ResetComboScore,ComboTimeLimit, false);
}

void AMyWukongCharacter::ResetCombo()
{
	ComboCounter = 0;
}

void AMyWukongCharacter::ResetHeavyCombo()
{
	HeavyAttackComboCounter = 0;
}

void AMyWukongCharacter::SetupStimulusSource()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (StimulusSource)
	{
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterWithPerceptionSystem();
	}
}

// Called to bind functionality to input
void AMyWukongCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	//Player Movement
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyWukongCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyWukongCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMyWukongCharacter::TurnRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyWukongCharacter::LookUpRate);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//Dodging
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AMyWukongCharacter::Dodge);

	//Jumping
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyWukongCharacter::CustomJump);

	//Running
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMyWukongCharacter::Running);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMyWukongCharacter::StopRunning);

	//Combat
	/*PlayerInputComponent->BindAction("Recall", IE_Pressed, this, &AMyWukongCharacter::Recall);*/

	PlayerInputComponent->BindAction("MainAttack", IE_Pressed, this, &AMyWukongCharacter::MainAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AMyWukongCharacter::HeavyAttack);
}

void AMyWukongCharacter::ActivateRightWeapon()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Activate Weapon"));
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	//Debug for weapon collision
	/*FVector BoxExtent = RightWeaponCollision->GetScaledBoxExtent();
	FVector Location = RightWeaponCollision->GetComponentLocation();
	FQuat Rotation = RightWeaponCollision->GetComponentQuat();

	DrawDebugBox(GetWorld(),Location,BoxExtent,Rotation,FColor::Red,false, 2.1f );*/
}

void AMyWukongCharacter::DeactivateRightWeapon()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Deactivate Weapon"));
	RightWeaponCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	RightWeaponCollision->SetRelativeScale3D(FVector(1.25f, 1.0f, 4.0f));
	RightWeaponCollision->SetRelativeRotation(FRotator(0.0f, 15.0f, 0.0f));
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->UpdateOverlaps();
	bIsAttacking = false;
	bIsHeavyAttacking = false;
	bIsAirAttacking = false;
	CanDodge = true;
	bCanAttack = true;
	/*bCanHeavyAttack = true;*/

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement)
	{
		if (Movement->MovementMode != MOVE_Walking && !Movement->IsFalling())
		{
			Movement->SetMovementMode(MOVE_Walking);
		}
	}

	if (GetCharacterMovement()->MovementMode != MOVE_Walking)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

}

float AMyWukongCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDodging || bIsInHitReact)
	{
		return 0.0f;
	}

	if (Health - DamageAmount <= 0.0f)
	{
		Health = 0.0f;
		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DeathOfPlayer();
		return 0.0f;
	}
	else
	{
		if (FMath::FRand() <= HitReactChance)
		{
			PlayHitReaction(DamageCauser);
		}

		Health -= DamageAmount;
		ResetComboScore();
	}
	return DamageAmount;
}

FVector AMyWukongCharacter::GetHitDirectionFromAttacker(AActor* DamageCauser)
{
	if (!DamageCauser) return FVector::ForwardVector;

	FVector DirectionToAttacker = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	FVector LocalDirection = GetActorRotation().UnrotateVector(DirectionToAttacker);
	LocalDirection.Z = 0; 

	return LocalDirection.GetSafeNormal();
}

void AMyWukongCharacter::PlayDirectionalHitReaction(const FVector& HitDirection)
{
	if (!GetMesh() || !GetMesh()->GetAnimInstance()) return;

	UAnimMontage* MontageToPlay = nullptr;

	float ForwardDot = FVector::DotProduct(HitDirection, FVector::ForwardVector);
	float RightDot = FVector::DotProduct(HitDirection, FVector::RightVector);

	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		if (ForwardDot > 0)
		{
			MontageToPlay = HitReactionFrontMontage;
		}
		else
		{
			MontageToPlay = HitReactionBackMontage;
		}
	}
	else
	{
		if (RightDot > 0)
		{
			MontageToPlay = HitReactionRightMontage;
		}
		else
		{
			MontageToPlay = HitReactionLeftMontage;
		}
	}

	if (MontageToPlay)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		FOnMontageEnded HitReactEndDelegate;
		HitReactEndDelegate.BindUObject(this, &AMyWukongCharacter::OnHitReactionEnded);
		AnimInstance->Montage_SetEndDelegate(HitReactEndDelegate, MontageToPlay);

		float MontageLength = AnimInstance->Montage_Play(MontageToPlay);

		if (MontageLength <= 0.0f)
		{
			EndHitReaction();
			return;
		}

		float ResetTime = FMath::Max(MontageLength + 0.2f, 1.0f);

		GetWorld()->GetTimerManager().ClearTimer(HitReactTimer);

		GetWorld()->GetTimerManager().SetTimer(
			HitReactTimer,
			this,
			&AMyWukongCharacter::EndHitReaction,
			ResetTime,
			false
		);

	}
	else
	{
		EndHitReaction();
	}
}

void AMyWukongCharacter::PlayHitReaction(AActor* DamageCauser)
{
	if (bIsInHitReact) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (!HitReactionFrontMontage && !HitReactionBackMontage &&
		!HitReactionLeftMontage && !HitReactionRightMontage)
	{
		return;
	}

	bIsInHitReact = true;

	if (bIsAttacking || bIsHeavyAttacking)
	{
		AnimInstance->Montage_Stop(0.1f);
		bIsAttacking = false;
		bIsHeavyAttacking = false;
		DeactivateRightWeapon();
	}

	GetCharacterMovement()->DisableMovement();
	bCanAttack = false;
	bCanHeavyAttack = false;
	CanDodge = false;

	if (DamageCauser)
	{
		FVector HitDirection = GetHitDirectionFromAttacker(DamageCauser);
		PlayDirectionalHitReaction(HitDirection);
	}
	else
	{
		if (HitReactionFrontMontage)
		{
			PlayDirectionalHitReaction(FVector::ForwardVector);
		}
		else
		{
			EndHitReaction();
		}
	}
}

void AMyWukongCharacter::EndHitReaction()
{
	if (!bIsInHitReact)
		return;

	bIsInHitReact = false;
	bCanAttack = true;
	bCanHeavyAttack = true;
	CanDodge = true;

	if (GetCharacterMovement()->MovementMode != MOVE_Walking)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	GetWorld()->GetTimerManager().ClearTimer(HitReactTimer);
}

void AMyWukongCharacter::OnHitReactionEnded(UAnimMontage* Montage, bool bInterrupted)
{
	EndHitReaction();
}

void AMyWukongCharacter::ResetCombatState()
{
	
	bIsInHitReact = false;
	bIsAttacking = false;
	bIsHeavyAttacking = false;
	bIsAirAttacking = false;
	bIsDodging = false;
	bCanAttack = true;
	bCanHeavyAttack = true;
	CanDodge = true;

	GetWorld()->GetTimerManager().ClearTimer(HitReactTimer);
	GetWorld()->GetTimerManager().ClearTimer(AttackCooldownTimer);
	GetWorld()->GetTimerManager().ClearTimer(HeavyAttackCooldownTimer);
	GetWorld()->GetTimerManager().ClearTimer(AttackInputBuffer);

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.1f);
	}

	DeactivateRightWeapon();

}

void AMyWukongCharacter::AddHealth(float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	float OldHealth = Health;
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	float ActualHeal = Health - OldHealth;

	if (GEngine && ActualHeal > 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("+%.0f Health"), ActualHeal)
		);
	}
}
