// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindStrafeLocation.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UBTT_FindStrafeLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTT_FindStrafeLocation(const FObjectInitializer& ObjectInitalizer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	UEnvQuery* LocationSeekerQuery;

	FEnvQueryRequest LocationSeekerQueryRequest;

	void LocationSeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	class AEnemyAIController* Controller;

	FVector StrafeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	float Distance = 500.0f;

	bool IsDistanceGreaterThanX(FVector Location);
};
