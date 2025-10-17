// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Enemy/EnemyAIController.h"
#include "../../Public/Enemy/Enemy.h"
#include "../../Public/Character/MyWukongCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController(FObjectInitializer const& ObjectInitializer)
{
    SetupPerceptionSystem();
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (AEnemy* const Enemy = Cast<AEnemy>(InPawn))
    {
        if (UBehaviorTree* const Tree = Enemy->GetBehaviorTree())
        {
            UBlackboardComponent* BlackboardComp = nullptr;

            if (UseBlackboard(Tree->BlackboardAsset, BlackboardComp))
            {
                Blackboard = BlackboardComp;
                RunBehaviorTree(Tree);
            }
        }
    }
}

void AEnemyAIController::SetupPerceptionSystem()
{
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    if (SightConfig)
    {
        SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
        SightConfig->SightRadius = 800.0f;
        SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->SetMaxAge(5.0f);
        SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.0f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

        GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
        GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetFound);
        GetPerceptionComponent()->ConfigureSense(*SightConfig);
    }
}

void AEnemyAIController::OnTargetFound(AActor* Actor, FAIStimulus const Stimulus)
{
    if (auto* const Player = Cast<AMyWukongCharacter>(Actor))
    {
        GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", Stimulus.WasSuccessfullySensed());
    }
}
