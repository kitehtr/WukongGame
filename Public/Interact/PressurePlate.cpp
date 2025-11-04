#include "Project/WukongGame/Public/Interact/PressurePlate.h"
#include "Project/WukongGame/Public/Interact/door.h"

APressurePlate::APressurePlate()
{

    PrimaryActorTick.bCanEverTick = false;

    PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
    RootComponent = PlateMesh;

    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    TriggerVolume->SetupAttachment(RootComponent);
    TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void APressurePlate::BeginPlay()
{
	Super::BeginPlay();
    

    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnOverlapBegin);
    TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::OnOverlapEnd);
}

void APressurePlate::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
   if (OtherActor && OtherActor != this)
    {
        GetWorld()->GetTimerManager().ClearTimer(CloseDoorTimer);

        if (DoorToOpen)
        {
            Adoor* Door = Cast<Adoor>(DoorToOpen);
            if (Door)
            {
                Door->OpenDoor();
            }
        }

        bIsActive = true;
    }
}

void APressurePlate::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this)
    {

        GetWorld()->GetTimerManager().SetTimer(CloseDoorTimer, this, &APressurePlate::CloseDoor, CloseDelay, false);
    }
}

void APressurePlate::CloseDoor()
{
    if (DoorToOpen)
    {
        Adoor* Door = Cast<Adoor>(DoorToOpen);
        if (Door)
        {
            Door->CloseDoor();
        }
    }

    bIsActive = false;
}

