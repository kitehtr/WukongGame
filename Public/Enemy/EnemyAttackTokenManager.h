// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyAttackTokenManager.generated.h"

UCLASS()
class WUKONG_API AEnemyAttackTokenManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyAttackTokenManager();

	static AEnemyAttackTokenManager* GetInstance(UWorld* World);

	bool RequestAttackToken(AActor* Requester, float Priority = 1.0f);
	void ReleaseAttackToken(AActor* TokenHolder);
	void ForceReleaseAttackToken(AActor* TokenHolder);

	bool HasToken(AActor* Actor) const;
	int32 GetAvailableTokens() const;
	bool CanRequestToken() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	UPROPERTY(EditAnywhere, Category = "Token System")
	int32 MaxAttackTokens = 3;

	UPROPERTY(EditAnywhere, Category = "Token System")
	float TokenReassignDelay = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Token System")
	float MaxTokenHoldTime = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Token System")
	bool bUseDistancePriority = true;

	TMap<AActor*, float> ActiveTokenHolders;

	TMap<AActor*, FTimerHandle> TokenExpirationTimers;

	static AEnemyAttackTokenManager* Instance;

	void ExpireToken(AActor* TokenHolder);
	float CalculatePriority(AActor* Requester);
	AActor* FindLowestPriorityTokenHolder();
};
