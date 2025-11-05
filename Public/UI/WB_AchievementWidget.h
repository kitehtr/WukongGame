// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WB_AchievementWidget.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UWB_AchievementWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Achievement")
    void ShowAchievement(const FString& AchievementName);

    UFUNCTION(BlueprintCallable, Category = "Achievement")
    void StartAutoRemoveTimer();
};
