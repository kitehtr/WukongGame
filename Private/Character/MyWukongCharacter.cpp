// Fill out your copyright notice in the Description page of Project Settings.

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
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyWukongCharacter::AMyWukongCharacter() :
	DefaultTurnRate(45.0f),
	DefaultLookUpRate(45.0f),
	WalkSpeed(750.0f),
	RunSpeed(1500.0f),
	BaseDamage(10.0f),
	Health(100.0f),
	MaxHealth(100.0f)

{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	FollowCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	FollowCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	FollowCameraComponent->bUsePawnControlRotation = false;

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

void AMyWukongCharacter::HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& a_pBranchingPayload)
{
	if (NotifyName.ToString() == "Dodge")
	{
		bIsDodging = false;
	}
}

// Called when the game starts or when spawned
void AMyWukongCharacter::BeginPlay()
{
	Super::BeginPlay();

	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AMyWukongCharacter::OnRightWeaponOverlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
	if (pAnimInst != nullptr)
	{
		pAnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &AMyWukongCharacter::HandleOnMontageNotifyBegin);
	}
}

void AMyWukongCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	if (JumpCurrentCount == 1)
	{
		LaunchCharacter(FVector(0, 0, 1000.0f), false, true);
	}
	else if (JumpCurrentCount == 2)
	{
		CanDodge = true;
		bIsDoubleJumping = true;
		LaunchCharacter(FVector(0, 0, 1000.0f), false, true);
	}
	else if (JumpCurrentCount == 3)
	{
		CanDodge = true;
		bIsTripleJumping = true;
		LaunchCharacter(FVector(0, 0, 1000.0f), false, true);
	}
}

void AMyWukongCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	GetCharacterMovement()->GravityScale = 1.25f;
	CanDodge = true;
	bIsDoubleJumping = false;
	bIsTripleJumping = false;
	bCanAttack = true;
	bIsAttacking = false;
	bIsHeavyAttacking = false;

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
	if (bIsAttacking || bIsHeavyAttacking)
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
	if (bIsAttacking || bIsHeavyAttacking)
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
	if (bIsDodging || bIsAttacking || !bCanAttack || bIsHeavyAttacking)
	{
		if (!bAttackDelay) 
		{
			bAttackDelay = true;
			GetWorld()->GetTimerManager().SetTimer(AttackInputBuffer, this, &AMyWukongCharacter::ClearAttackDelay, 0.2f, false);
		}
		return;
	}

	if (bAttackDelay)
	{
		bAttackDelay = false;
		GetWorld()->GetTimerManager().ClearTimer(AttackInputBuffer);
	}

	if (GetCharacterMovement()->IsFalling())
	{
		AirAttack();
		return;
	}

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

		GetCharacterMovement()->DisableMovement();

		FOnMontageEnded AttackEndedDelegate;
		AttackEndedDelegate.BindUObject(this, &AMyWukongCharacter::OnAttackEnded);
		AnimInstance->Montage_SetEndDelegate(AttackEndedDelegate, MainAttackMontage);

		AnimInstance->Montage_Play(MainAttackMontage);
		AnimInstance->Montage_JumpToSection(SectionName, MainAttackMontage);

		GetWorld()->GetTimerManager().SetTimer(LightComboResetTimer, this, &AMyWukongCharacter::ResetCombo, 4.0f, false);

		GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimer, [this]() {bCanAttack = true;}, 0.5f, false);
	}
	else
	{
		bIsAttacking = false;
		bCanAttack = true; 
	}
}

void AMyWukongCharacter::OnAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;
	bIsHeavyAttacking = false;

	if (bAttackDelay)
	{
		GetWorld()->GetTimerManager().SetTimer(AttackInputBuffer, this, &AMyWukongCharacter::BufferAttackInput, 0.1f, false);
	}

	if (GetCharacterMovement()->MovementMode != MOVE_Walking)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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

void AMyWukongCharacter::ClearAttackDelay()
{
	bAttackDelay = false;
	GetWorld()->GetTimerManager().ClearTimer(AttackInputBuffer);
}

void AMyWukongCharacter::AirAttack()
{
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

		AnimInstance->Montage_Play(AirAttackMontage);

		GetCharacterMovement()->Velocity = FVector::ZeroVector; 
		LaunchCharacter(FVector(0, 0, -5000.0f), true, true);
		
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
	if (bIsDodging || bIsAttacking || !bCanAttack || bIsHeavyAttacking)
	{
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
		AlreadyHitActors.Empty();
		bIsHeavyAttacking = true;
		CurrentAttackType = EAttackType::HeavyAttack;
		int32 const HeavySectionCount = HeavyAttackMontage->CompositeSections.Num();

		FName const HeavySectionName = GetHeavyAttackSectionName(HeavySectionCount);

		GetCharacterMovement()->DisableMovement();

		AnimInstance->Montage_Play(HeavyAttackMontage);
		AnimInstance->Montage_JumpToSection(HeavySectionName, HeavyAttackMontage);
		
		GetWorld()->GetTimerManager().SetTimer(HeavyComboResetTimer, this, &AMyWukongCharacter::ResetHeavyCombo, 2.0f, false);
	}
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

		//Check if the actor was already hit
		if (AlreadyHitActors.Contains(SweepResult.GetActor()))
		{
			return;
		}

		// Add to the set to prevent multiple hits
		AlreadyHitActors.Add(SweepResult.GetActor());

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
			
	}
	
}

void AMyWukongCharacter::ResetCombo()
{
	ComboCounter = 0;
}

void AMyWukongCharacter::ResetHeavyCombo()
{
	HeavyAttackComboCounter = 0;
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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

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
	FTimerHandle MovementTimer;
	GetWorld()->GetTimerManager().SetTimer(MovementTimer,this,&AMyWukongCharacter::EnableMovement, DelayTimeForAttack, false);
	/*GetCharacterMovement()->SetMovementMode(MOVE_Walking);*/
}

float AMyWukongCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDodging)
	{
		return 0.0f;
	}
	if (Health - DamageAmount <= 0.0f)
	{
		Health = 0.0f;

		/*GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetVisibility(false);*/
		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DeathOfPlayer();
		return 0.0f;
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

