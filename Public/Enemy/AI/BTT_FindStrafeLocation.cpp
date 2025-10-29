// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTT_FindStrafeLocation.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "../../../Public/Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "../../../Public/Character/MyWukongCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindStrafeLocation::UBTT_FindStrafeLocation(const FObjectInitializer& ObjectInitalizer)
{
	bCreateNodeInstance = true;
	NodeName = "Find Strafe Location";
}

EBTNodeResult::Type UBTT_FindStrafeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyAIController* LocalController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
    if (LocalController && LocationSeekerQuery)
    {
        APawn* ControlledPawn = LocalController->GetPawn();
        if (ControlledPawn)
        {
            this->Controller = LocalController;

            LocationSeekerQueryRequest = FEnvQueryRequest(LocationSeekerQuery, ControlledPawn);

            LocationSeekerQueryRequest.Execute(EEnvQueryRunMode::RandomBest25Pct, this,&UBTT_FindStrafeLocation::LocationSeekerQueryFinished);

            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}

void UBTT_FindStrafeLocation::LocationSeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
    if (!Controller || !Result.IsValid())
    {
        return;
    }

    int32 Index = 0;
    float CurrentBestScore = -1.0f;
    TArray<FVector> Locations;
    Result->GetAllAsLocations(Locations);

    bool bFoundValidLocation = false; 

    for (auto& Loc : Locations)
    {
        if (IsDistanceGreaterThanX(Loc) && Result->GetItemScore(Index) > CurrentBestScore)
        {
            StrafeLocation = Loc;
            CurrentBestScore = Result->GetItemScore(Index);
            bFoundValidLocation = true; 
        }
        Index++;
    }

    if (bFoundValidLocation)
    {
        if (UBlackboardComponent* BlackboardComp = Controller->GetBlackboardComponent())
        {
            BlackboardComp->SetValueAsVector("MoveToLocation", StrafeLocation);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid strafe location found"));
    }
}

bool UBTT_FindStrafeLocation::IsDistanceGreaterThanX(FVector Location)
{
    if (!Controller) return false;

    UWorld* World = Controller->GetWorld();
    if (!World) return false;

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, AEnemy::StaticClass(), FoundActors);

    if (FoundActors.Num() <= 1) return true;

    bool ConsiderLocation = true;
    for (AActor* EnemyActor : FoundActors)
    {
        if (EnemyActor && EnemyActor != Controller->GetPawn()) 
        {
            const float CalculatedDistance = (Location - EnemyActor->GetActorLocation()).Size();
            if (CalculatedDistance <= Distance)
            {
                ConsiderLocation = false;
                break;
            }
        }
    }

    return ConsiderLocation;
}

