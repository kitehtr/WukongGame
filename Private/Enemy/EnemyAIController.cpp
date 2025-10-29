// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Enemy/EnemyAIController.h"
#include "../../Public/Enemy/Enemy.h"
#include "../../Public/Character/MyWukongCharacter.h"
#include "GameFramework/Character.h" 
#include "GameFramework/CharacterMovementComponent.h" 
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig.h"

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

    if (ACharacter* PossessedCharacter = Cast<ACharacter>(InPawn))
    {
        if (UCharacterMovementComponent* MovementComp = PossessedCharacter->GetCharacterMovement())
        {
            MovementComp->bOrientRotationToMovement = false;
            MovementComp->bUseControllerDesiredRotation = true;
            MovementComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
        }
        PossessedCharacter->bUseControllerRotationYaw = true;
    }

    if (AEnemy* const Enemy = Cast<AEnemy>(InPawn))
    {
        if (UBehaviorTree* const Tree = Enemy->GetBehaviorTree())
        {
            UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
            if (UseBlackboard(Tree->BlackboardAsset, BlackboardComp))
            {
                BlackboardComp->SetValueAsBool("CanSeePlayer", false);
                BlackboardComp->SetValueAsObject("TargetActor", nullptr);
                RunBehaviorTree(Tree);
            }
        }
    }
}

void AEnemyAIController::SetupPerceptionSystem()
{
    UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
    SetPerceptionComponent(*PerceptionComp);

    if (!GetPerceptionComponent()) return;

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    if (!SightConfig) return;

    SightConfig->SightRadius = 2000.0f;
    SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.0f;
    SightConfig->SetMaxAge(15.0f);
    SightConfig->PeripheralVisionAngleDegrees = 180.0f;
    SightConfig->AutoSuccessRangeFromLastSeenLocation = 120.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
    GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetFound);
    GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void AEnemyAIController::OnTargetFound(AActor* Actor, FAIStimulus const Stimulus)
{
    if (!Actor) return;

    if (auto* const Player = Cast<AMyWukongCharacter>(Actor))
    {
        UBlackboardComponent* const BBC = GetBlackboardComponent();
        if (!BBC) return;

        BBC->SetValueAsBool("CanSeePlayer", Stimulus.WasSuccessfullySensed());

        if (Stimulus.WasSuccessfullySensed())
        {
            BBC->SetValueAsObject("TargetActor", Player);
            BBC->SetValueAsVector("TargetLocation", Actor->GetActorLocation());

            SetFocus(Player, EAIFocusPriority::Gameplay);

            if (APawn* ControlledPawn = GetPawn())
            {
                ControlledPawn->bUseControllerRotationYaw = true;
            }
        }
        else
        {
            BBC->SetValueAsObject("TargetActor", nullptr);
            BBC->SetValueAsVector("LastKnownLocation", Stimulus.StimulusLocation);

            ClearFocus(EAIFocusPriority::Gameplay);

            if (APawn* ControlledPawn = GetPawn())
            {
                ControlledPawn->bUseControllerRotationYaw = false;
            }
        }
    }
}