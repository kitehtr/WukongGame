// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Breakables/BreakableItem.h"

// Sets default values
ABreakableItem::ABreakableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABreakableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABreakableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

