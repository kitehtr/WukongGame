// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Character/MyWukongCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Public/Enemy/MeleeHitInterface.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyWukongCharacter::AMyWukongCharacter() :
	DefaultTurnRate(45.0f),
	DefaultLookUpRate(45.0f),
	WalkSpeed(600.0f),
	RunSpeed(1200.0f),
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

	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.3f;
	JumpMaxCount = 2;

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));
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
}

void AMyWukongCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AMyWukongCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
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

void AMyWukongCharacter::Recall()
{
	PlayAnimMontage(RecallMontage, FName("Recall"));
}

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
	if (bIsAttacking)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MainAttackMontage)
	{
		AlreadyHitActors.Empty();
		bIsAttacking = true;
		CurrentAttackType = EAttackType::BasicAttack;
		int32 const SectionCount = MainAttackMontage->CompositeSections.Num();

		FName const SectionName = GetAttackSectionName(SectionCount);

		GetCharacterMovement()->DisableMovement();

		/*int32 const SectionIndex = MainAttackMontage->GetSectionIndex(SectionName);
		float const SectionLength = MainAttackMontage->GetSectionLength(SectionIndex);*/

		AnimInstance->Montage_Play(MainAttackMontage);
		AnimInstance->Montage_JumpToSection(SectionName, MainAttackMontage);
	}
}

void AMyWukongCharacter::HeavyAttack()
{
	if (bIsAttacking || bIsHeavyAttacking)
	{
		return;
	}
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

	//Jumping
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Running
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMyWukongCharacter::Running);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMyWukongCharacter::StopRunning);

	//Combat
	PlayerInputComponent->BindAction("Recall", IE_Pressed, this, &AMyWukongCharacter::Recall);

	PlayerInputComponent->BindAction("MainAttack", IE_Pressed, this, &AMyWukongCharacter::MainAttack);

	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AMyWukongCharacter::HeavyAttack);
}

void AMyWukongCharacter::ActivateRightWeapon()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Activate Weapon"));
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMyWukongCharacter::DeactivateRightWeapon()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Deactivate Weapon"));
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bIsAttacking = false;
	bIsHeavyAttacking = false;
	FTimerHandle MovementTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle,this,&AMyWukongCharacter::EnableMovement, DelayTimeForAttack, false);
	/*GetCharacterMovement()->SetMovementMode(MOVE_Walking);*/
}

float AMyWukongCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.0f)
	{
		Health = 0.0f;

		/*GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetVisibility(false);*/
		GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DeathOfPlayer();
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

