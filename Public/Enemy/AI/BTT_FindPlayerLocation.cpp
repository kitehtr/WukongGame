// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTT_FindPlayerLocation.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTT_FindPlayerLocation::UBTT_FindPlayerLocation(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase{ ObjectInitializer }
{
	NodeName = "Find Player Location";
}

EBTNodeResult::Type UBTT_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    if (auto* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
    {
        auto const PlayerLocation = Player->GetActorLocation();
        if (SearchRandom)
        {
            FNavLocation Loc;
            if (auto* const NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
            {
                if (NavSystem->GetRandomPointInNavigableRadius(PlayerLocation, SearchRadius, Loc))
                {
                    OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);
                    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                    return EBTNodeResult::Succeeded;
                }
            }
        }
        else
        {
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerLocation);
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}