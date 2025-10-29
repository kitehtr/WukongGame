// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/Enemy/AI/BTT_ClearFocus.h"
#include "../../../Public/Enemy/EnemyAIController.h"

UBTT_ClearFocus::UBTT_ClearFocus()
{
	NodeName = TEXT("Clear Focus");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTT_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	// Clear the focus
	AIController->ClearFocus(EAIFocusPriority::Gameplay);

	// Optionally stop forced rotation
	if (!bClearFocusOnly)
	{
		if (APawn* ControlledPawn = AIController->GetPawn())
		{
			ControlledPawn->bUseControllerRotationYaw = false;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("ClearFocus: Focus cleared for AI"));

	return EBTNodeResult::Succeeded;
}
