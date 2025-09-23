// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyWukongCharacter.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NotifyStateRightWeapon.generated.h"

class AMyWukongCharacter;
/**
 * 
 */
UCLASS()
class WUKONG_API UNotifyStateRightWeapon : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	AMyWukongCharacter* WukongCharacter;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
