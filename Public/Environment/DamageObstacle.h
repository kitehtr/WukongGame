// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageObstacle.generated.h"

UCLASS()
class WUKONG_API ADamageObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageObstacle();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBoxComponent* TriggerVolume;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* ObstacleMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float DamageAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float DamageInterval;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void ApplyPeriodicDamage();

protected:
	virtual void BeginPlay() override;

private:
    FTimerHandle DamageTimerHandle;
    TArray<AActor*> OverlappingActors;

};
