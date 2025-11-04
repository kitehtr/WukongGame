#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "door.generated.h"

UCLASS()
class WUKONG_API Adoor : public AActor
{
	GENERATED_BODY()
	
public:	
	Adoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseDoor();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* DoorMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    float OpenHeight = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    float MoveSpeed = 300.0f;

private:
    FVector InitialLocation;
    FVector TargetLocation;
    bool bIsOpening;

    virtual void Tick(float DeltaTime) override;


};
