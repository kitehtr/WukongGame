// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTS_UpdateTargetLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTS_UpdateTargetLocation::UBTS_UpdateTargetLocation()
{
	NodeName = TEXT("Update Target Location");
	Interval = 0.5f; 
	RandomDeviation = 0.1f;

	bNotifyBecomeRelevant = true;
	bNotifyOnSearch = true;
	bNotifyCeaseRelevant = false;

}

void UBTS_UpdateTargetLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	UObject* TargetObject = BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(TargetObject);

	if (TargetActor)
	{
		FVector TargetLocation = TargetActor->GetActorLocation();
		BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), TargetLocation);
	}
}
