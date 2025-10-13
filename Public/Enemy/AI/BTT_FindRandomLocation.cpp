// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTT_FindRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTT_FindRandomLocation::UBTT_FindRandomLocation(FObjectInitializer const&)
{
	NodeName = "Find Random Location in NavMesh";
}

EBTNodeResult::Type UBTT_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const Cont = Cast<AEnemyAIController>(OwnerComp.GetAIOwner()))
	{
		if (APawn* const Enemy = Cont->GetPawn())
		{
			const FVector Origin = Enemy->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Origin: %s"), *Origin.ToString());

			if (auto* const NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation Loc;
				if (NavSystem->GetRandomPointInNavigableRadius(Origin, SearchRadius, Loc))
				{
					UE_LOG(LogTemp, Warning, TEXT("Found valid Nav point: %s"), *Loc.Location.ToString());
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("No valid Nav point found within radius %.1f"), SearchRadius);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("NavSystem is NULL"));
			}
		}
	}
	return EBTNodeResult::Failed;
}