// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTT_MoveToLocation.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"

UBTT_MoveToLocation::UBTT_MoveToLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Move To Location";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_MoveToLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	FVector TargetLocation = BlackboardComp->GetValueAsVector(GetSelectedBlackboardKey());

	if (TargetLocation == FAISystem::InvalidLocation)
	{
		return EBTNodeResult::Failed;
	}

	LastTargetLocation = TargetLocation;
	TimeSinceLastUpdate = 0.0f;

	FAIMoveRequest MoveRequest(TargetLocation);
	MoveRequest.SetAcceptanceRadius(AcceptableRadius);
	MoveRequest.SetAllowPartialPath(false);

	FPathFollowingRequestResult RequestResult = AIController->MoveTo(MoveRequest);

	if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
	{
		MoveRequestID = RequestResult.MoveId;
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTT_MoveToLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector CurrentTargetLocation = BlackboardComp->GetValueAsVector(GetSelectedBlackboardKey());

	if (CurrentTargetLocation == FAISystem::InvalidLocation)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	float DistanceToTarget = FVector::Dist(ControlledPawn->GetActorLocation(), CurrentTargetLocation);
	if (DistanceToTarget <= AcceptableRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	TimeSinceLastUpdate += DeltaSeconds;
	float TargetMovedDistance = FVector::Dist(LastTargetLocation, CurrentTargetLocation);

	bool bShouldUpdatePath = false;

	if (TargetMovedDistance > PathUpdateThreshold)
	{
		bShouldUpdatePath = true;
	}

	else if (TimeSinceLastUpdate >= MinUpdateInterval && TargetMovedDistance > 10.0f)
	{
		bShouldUpdatePath = true;
	}

	if (bShouldUpdatePath)
	{
		/*UE_LOG(LogTemp, Log, TEXT("MoveTo: Updating path - Target moved %f units"), TargetMovedDistance);*/

		LastTargetLocation = CurrentTargetLocation;
		TimeSinceLastUpdate = 0.0f;

		FAIMoveRequest MoveRequest(CurrentTargetLocation);
		MoveRequest.SetAcceptanceRadius(AcceptableRadius);
		MoveRequest.SetAllowPartialPath(false);

		FPathFollowingRequestResult RequestResult = AIController->MoveTo(MoveRequest);

		if (RequestResult.Code != EPathFollowingRequestResult::RequestSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveTo: Failed to update path"));
		}
		else
		{
			MoveRequestID = RequestResult.MoveId;
		}
	}

	if (UPathFollowingComponent* PFComp = AIController->GetPathFollowingComponent())
	{
		if (PFComp->GetStatus() == EPathFollowingStatus::Idle)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
	}
}

void UBTT_MoveToLocation::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (UPathFollowingComponent* PFComp = AIController->GetPathFollowingComponent())
		{
			PFComp->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
		}
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}