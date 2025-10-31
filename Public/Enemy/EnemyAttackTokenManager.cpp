#include "Project/WukongGame/Public/Enemy/EnemyAttackTokenManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

AEnemyAttackTokenManager* AEnemyAttackTokenManager::Instance = nullptr;

AEnemyAttackTokenManager::AEnemyAttackTokenManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAttackTokenManager::BeginPlay()
{
	Super::BeginPlay();
	Instance = this;
}

void AEnemyAttackTokenManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (auto& Pair : TokenExpirationTimers)
	{
		if (Pair.Value.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(Pair.Value);
		}
	}

	if (Instance == this)
	{
		Instance = nullptr;
	}
}

AEnemyAttackTokenManager* AEnemyAttackTokenManager::GetInstance(UWorld* World)
{
	if (!Instance && World)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, AEnemyAttackTokenManager::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0)
		{
			Instance = Cast<AEnemyAttackTokenManager>(FoundActors[0]);
		}
		else
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Name = FName("AttackTokenManager");
			Instance = World->SpawnActor<AEnemyAttackTokenManager>(AEnemyAttackTokenManager::StaticClass(), SpawnParams);
		}
	}

	return Instance;
}

bool AEnemyAttackTokenManager::RequestAttackToken(AActor* Requester, float Priority)
{
	if (!Requester)
	{
		return false;
	}

	if (HasToken(Requester))
	{
		return true;
	}

	if (ActiveTokenHolders.Num() < MaxAttackTokens)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		ActiveTokenHolders.Add(Requester, CurrentTime);

		FTimerHandle ExpirationTimer;
		GetWorld()->GetTimerManager().SetTimer(ExpirationTimer,[this, Requester]() { ExpireToken(Requester); },MaxTokenHoldTime,false);
		TokenExpirationTimers.Add(Requester, ExpirationTimer);

		return true;
	}

	if (bUseDistancePriority)
	{
		float RequesterPriority = CalculatePriority(Requester);
		AActor* LowestPriorityHolder = FindLowestPriorityTokenHolder();

		if (LowestPriorityHolder)
		{
			float LowestPriority = CalculatePriority(LowestPriorityHolder);

			if (RequesterPriority > LowestPriority * 1.5f) 
			{
				ForceReleaseAttackToken(LowestPriorityHolder);
				return RequestAttackToken(Requester, Priority);
			}
		}
	}
	return false;
}

void AEnemyAttackTokenManager::ReleaseAttackToken(AActor* TokenHolder)
{
	if (!TokenHolder || !HasToken(TokenHolder))
	{
		return;
	}
	if (TokenExpirationTimers.Contains(TokenHolder))
	{
		FTimerHandle TimerHandle = TokenExpirationTimers[TokenHolder];
		if (TimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		}
		TokenExpirationTimers.Remove(TokenHolder);
	}

	ActiveTokenHolders.Remove(TokenHolder);
}

void AEnemyAttackTokenManager::ForceReleaseAttackToken(AActor* TokenHolder)
{
	ReleaseAttackToken(TokenHolder);
}

bool AEnemyAttackTokenManager::HasToken(AActor* Actor) const
{
	return ActiveTokenHolders.Contains(Actor);
}

int32 AEnemyAttackTokenManager::GetAvailableTokens() const
{
	return MaxAttackTokens - ActiveTokenHolders.Num();
}

bool AEnemyAttackTokenManager::CanRequestToken() const
{
	return ActiveTokenHolders.Num() < MaxAttackTokens;
}

void AEnemyAttackTokenManager::ExpireToken(AActor* TokenHolder)
{
	if (!TokenHolder)
	{
		return;
	}

	ReleaseAttackToken(TokenHolder);
}

float AEnemyAttackTokenManager::CalculatePriority(AActor* Requester)
{
	if (!Requester)
	{
		return 0.0f;
	}

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
	{
		return 1.0f;
	}

	float Distance = FVector::Dist(Requester->GetActorLocation(), Player->GetActorLocation());

	float Priority = FMath::Clamp(1.0f - (Distance / 3000.0f), 0.1f, 1.0f);

	return Priority;
}

AActor* AEnemyAttackTokenManager::FindLowestPriorityTokenHolder()
{
	AActor* LowestPriorityActor = nullptr;
	float LowestPriority = FLT_MAX;

	for (auto& Pair : ActiveTokenHolders)
	{
		float Priority = CalculatePriority(Pair.Key);
		if (Priority < LowestPriority)
		{
			LowestPriority = Priority;
			LowestPriorityActor = Pair.Key;
		}
	}

	return LowestPriorityActor;
}

