// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Character/WukongAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../Public/Character/MyWukongCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWukongAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (WukongCharacter == nullptr)
	{
		WukongCharacter = Cast<AMyWukongCharacter>(TryGetPawnOwner());
	}

	if (WukongCharacter)
	{
		FVector Velocity{ WukongCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = WukongCharacter->GetCharacterMovement()->IsFalling();

		bIsDoubleJumping = WukongCharacter->bIsDoubleJumping;

		bIsTripleJumping = WukongCharacter->bIsTripleJumping;

		FRotator const AimRotation = WukongCharacter->GetBaseAimRotation();
		FRotator const MovementRotation = UKismetMathLibrary::MakeRotFromX(WukongCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	}
}


void UWukongAnimInstance::NativeInitializeAnimation()
{
	WukongCharacter = Cast<AMyWukongCharacter>(TryGetPawnOwner());
}
