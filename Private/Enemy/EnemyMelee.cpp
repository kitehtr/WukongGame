// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Enemy/EnemyMelee.h"
#include "Project/WukongGame/Public/Enemy/EnemyAttackTokenManager.h"
#include "../../Public/Character/MyWukongCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../../Public/Enemy/EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyMelee::AEnemyMelee()
{
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponSocket"));
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyMelee::ActivateRightWeapon()
{
	/*GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Enemy Activate Weapon"));*/
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
 
void AEnemyMelee::DeactivateRightWeapon()
{
	/*GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Enemy Deactivate Weapon"));*/
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();

	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMelee::OnRightWeaponOverlap);

	TokenManager = AEnemyAttackTokenManager::GetInstance(GetWorld());
	if (!TokenManager)
	{
		TokenManager = AEnemyAttackTokenManager::GetInstance(GetWorld());
	}
}

void AEnemyMelee::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bHasAttackToken && TokenManager)
	{
		TokenManager->ReleaseAttackToken(this);
		bHasAttackToken = false;
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyMelee::MeleeAttack()
{
	if (!RequestAttackToken())
	{
		return;
	}

	AlreadyHitActors.Empty();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MeleeAttackMontage)
	{
		int32 const SectionCount = MeleeAttackMontage->CompositeSections.Num();

		FName const SectionName = GetAttackSectionName(SectionCount);
		int32 const SectionIndex = MeleeAttackMontage->GetSectionIndex(SectionName);
		float const SectionLength = MeleeAttackMontage->GetSectionLength(SectionIndex);

		GetCharacterMovement()->DisableMovement();

		AnimInstance->Montage_Play(MeleeAttackMontage);
		AnimInstance->Montage_JumpToSection(SectionName, MeleeAttackMontage);

		GetWorldTimerManager().SetTimer(
			TimerAttack,[this](){ResetMovementWalking();ReleaseAttackToken();},SectionLength,false);}
	else
	{
		ReleaseAttackToken();
	}
}

void AEnemyMelee::ResetMovementWalking()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

FName AEnemyMelee::GetAttackSectionName(int32 SectionCount)
{
	FName SectionName;
	const int32 Section{ FMath::RandRange(1, SectionCount) };

	switch (Section)
	{
	case 1:
		SectionName = FName("Attack1");
		break;
	case 2:
		SectionName = FName("Attack2");
		break;
	default:
		SectionName = FName("Attack1");
		break;
	}
	
	return SectionName;
}

void AEnemyMelee::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (OtherActor == nullptr)
	{
		return;
	}

	auto Character = Cast<AMyWukongCharacter>(OtherActor);
	if (Character)
	{
		if (AlreadyHitActors.Contains(SweepResult.GetActor()))
		{
			return;
		}
		AlreadyHitActors.Add(SweepResult.GetActor());

		UGameplayStatics::ApplyDamage(Character, GetBaseDamage(), EnemyAIController, this, UDamageType::StaticClass());
	}
}
void AEnemyMelee::MainMeleeAttack()
{
	MeleeAttack();
}

bool AEnemyMelee::RequestAttackToken()
{
	if (!TokenManager)
	{
		return false;
	}

	if (bHasAttackToken)
	{
		return true;
	}

	bHasAttackToken = TokenManager->RequestAttackToken(this);
	return bHasAttackToken;
}

void AEnemyMelee::ReleaseAttackToken()
{
	if (bHasAttackToken && TokenManager)
	{
		TokenManager->ReleaseAttackToken(this);
		bHasAttackToken = false;
	}
}
