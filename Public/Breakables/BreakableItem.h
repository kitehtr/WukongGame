// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableItem.generated.h"

UCLASS()
class WUKONG_API ABreakableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABreakableItem();

	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void BreakObject();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable")
	float Health = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Breakable")
	float MaxHealth = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	float HealthReward = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	float RewardRadius = 200.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	bool bAutoCollectReward = true; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* BreakParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* BreakSound;

	bool bIsBroken = false;

};
