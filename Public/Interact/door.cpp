#include "Project/WukongGame/Public/Interact/door.h"

Adoor::Adoor()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = DoorMesh;

	bIsOpening = false;
	SetActorTickEnabled(false);
}

void Adoor::OpenDoor()
{
	TargetLocation = InitialLocation + FVector(0, 0, OpenHeight);
	bIsOpening = true;
	SetActorTickEnabled(true);
}

void Adoor::CloseDoor()
{
	TargetLocation = InitialLocation;
	bIsOpening = false;
	SetActorTickEnabled(true);
}

void Adoor::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLocation = GetActorLocation();
	TargetLocation = InitialLocation;
}

void Adoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);

	SetActorLocation(NewLocation);

	if (FVector::Dist(NewLocation, TargetLocation) < 1.0f)
	{
		SetActorTickEnabled(false);
	}
}

