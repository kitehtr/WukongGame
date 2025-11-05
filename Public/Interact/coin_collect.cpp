#include "Project/WukongGame/Public/Interact/coin_collect.h"
#include "Kismet/GameplayStatics.h"
#include "../../Public/Character/MyWukongCharacter.h"
#include "Components/SphereComponent.h"

Acoin_collect::Acoin_collect()
{
	PrimaryActorTick.bCanEverTick = true;

    CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
    RootComponent = CollectionSphere;
    CollectionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
    CoinMesh->SetupAttachment(RootComponent);
    CoinMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CoinValue = 1;

    CollectionSphere->OnComponentBeginOverlap.AddDynamic(this, &Acoin_collect::OnOverlapBegin);
}


void Acoin_collect::BeginPlay()
{
	Super::BeginPlay();
	
}

void Acoin_collect::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AMyWukongCharacter* Player = Cast<AMyWukongCharacter>(OtherActor);
    if (Player)
    {
        Player->AddCoin(CoinValue);

        if (CollectionSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, CollectionSound, GetActorLocation());
        }

        Destroy();
    }
}

