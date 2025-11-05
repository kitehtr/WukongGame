#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "coin_collect.generated.h"

UCLASS()
class WUKONG_API Acoin_collect : public AActor
{
	GENERATED_BODY()
	
public:	
	Acoin_collect();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* CollectionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* CoinMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
    int32 CoinValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
    class USoundBase* CollectionSound;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Coin")
    void OnCollected();

};
