#include "../../Public/Breakables/BreakableItem.h"
#include "../../Public/Character/MyWukongCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"

ABreakableItem::ABreakableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	MeshComponent->SetNotifyRigidBodyCollision(true);
	MeshComponent->SetGenerateOverlapEvents(true);
}

void ABreakableItem::BeginPlay()
{
	Super::BeginPlay();
	
	MeshComponent->OnComponentHit.AddDynamic(this, &ABreakableItem::OnHit);

	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ABreakableItem::OnOverlapBegin);
}

void ABreakableItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		UGameplayStatics::ApplyDamage(this,10.0f, nullptr,OtherActor,UDamageType::StaticClass());
	}
}

void ABreakableItem::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (NormalImpulse.Size() > 1000.0f && !bIsBroken)
	{
		UE_LOG(LogTemp, Warning, TEXT("Breakable object hit with force: %.2f"), NormalImpulse.Size());

	}
}

void ABreakableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABreakableItem::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsBroken)
	{
		return 0.0f;
	}

	Health -= DamageAmount;

	if (Health <= 0.0f)
	{
		BreakObject();
	}

	return DamageAmount;
}

void ABreakableItem::BreakObject()
{
	if (bIsBroken)
	{
		return;
	}

	bIsBroken = true;

	if (BreakParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BreakParticle,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (BreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BreakSound,
			GetActorLocation()
		);
	}

	if (bAutoCollectReward)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyWukongCharacter::StaticClass(), FoundActors);

		for (AActor* Actor : FoundActors)
		{
			AMyWukongCharacter* Player = Cast<AMyWukongCharacter>(Actor);
			if (Player)
			{
				float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

				if (Distance <= RewardRadius)
				{
					Player->AddHealth(HealthReward);

					UE_LOG(LogTemp, Log, TEXT("Player received %.1f health from breakable!"), HealthReward);
					break;
				}
			}
		}
	}

	Destroy();
}