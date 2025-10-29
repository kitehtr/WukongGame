// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTS_FocusTarget.h"
#include "../../../Public/Enemy/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_FocusTarget::UBTS_FocusTarget()
{
	NodeName = TEXT("Set Focus On Target");
	Interval = 0.5f;
	RandomDeviation = 0.1f;

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	bNotifyTick = true;
}

void UBTS_FocusTarget::OnBecomeRelevant(AAIController* OwnerController, uint8* NodeMemory)
{

	if (!OwnerController) return;

	UBlackboardComponent* BlackboardComp = OwnerController->GetBlackboardComponent();
	if (!BlackboardComp) return;

	UObject* TargetObject = BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey());
	AActor* TargetActor = Cast<AActor>(TargetObject);

	if (TargetActor)
	{
		OwnerController->SetFocus(TargetActor, EAIFocusPriority::Gameplay);

		if (APawn* ControlledPawn = OwnerController->GetPawn())
		{
			ControlledPawn->bUseControllerRotationYaw = true;
		}

		UE_LOG(LogTemp, Log, TEXT("FocusTarget Service: Focus set on %s"), *TargetActor->GetName());
	}
}

void UBTS_FocusTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	UObject* TargetObject = BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey());
	AActor* TargetActor = Cast<AActor>(TargetObject);

	if (TargetActor)
	{
		OwnerController->SetFocus(TargetActor, EAIFocusPriority::Gameplay);
	}
}

void UBTS_FocusTarget::OnCeaseRelevant(AAIController* OwnerController, uint8* NodeMemory)
{

	if (bClearFocusOnExit && OwnerController)
	{
		OwnerController->ClearFocus(EAIFocusPriority::Gameplay);

		if (APawn* ControlledPawn = OwnerController->GetPawn())
		{
			ControlledPawn->bUseControllerRotationYaw = false;
		}

	}
}
