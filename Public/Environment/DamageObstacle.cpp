#include "Project/WukongGame/Public/Environment/DamageObstacle.h"
#include "Components/BoxComponent.h"
#include "../../Public/Character/MyWukongCharacter.h"
#include "Kismet/GameplayStatics.h"

ADamageObstacle::ADamageObstacle()
{
	PrimaryActorTick.bCanEverTick = false;

    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    RootComponent = TriggerVolume;

    ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
    ObstacleMesh->SetupAttachment(RootComponent);

    DamageAmount = 10.0f;
    DamageInterval = 1.0f;

    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ADamageObstacle::OnOverlapBegin);
    TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ADamageObstacle::OnOverlapEnd);

}

void ADamageObstacle::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADamageObstacle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AMyWukongCharacter* Player = Cast<AMyWukongCharacter>(OtherActor);
    if (Player && !OverlappingActors.Contains(OtherActor))
    {
        OverlappingActors.Add(OtherActor);

        UGameplayStatics::ApplyDamage(Player, DamageAmount, nullptr, this, UDamageType::StaticClass());

        if (!GetWorld()->GetTimerManager().IsTimerActive(DamageTimerHandle))
        {
            GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &ADamageObstacle::ApplyPeriodicDamage, DamageInterval, true);
        }
    }
}

void ADamageObstacle::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AMyWukongCharacter* Player = Cast<AMyWukongCharacter>(OtherActor);
    if (Player)
    {
        OverlappingActors.Remove(OtherActor);

        if (OverlappingActors.Num() == 0)
        {
            GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
        }
    }
}

void ADamageObstacle::ApplyPeriodicDamage()
{
    for (AActor* Actor : OverlappingActors)
    {
        if (AMyWukongCharacter* Player = Cast<AMyWukongCharacter>(Actor))
        {
            UGameplayStatics::ApplyDamage(Player, DamageAmount, nullptr, this, UDamageType::StaticClass());
        }
    }
}

