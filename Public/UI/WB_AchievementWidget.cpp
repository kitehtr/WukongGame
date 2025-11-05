// Fill out your copyright notice in the Description page of Project Settings.


#include "Project/WukongGame/Public/UI/WB_AchievementWidget.h"

void UWB_AchievementWidget::StartAutoRemoveTimer()
{
    if (UWorld* World = GetWorld())
    {
        FTimerHandle TimerHandle;
        World->GetTimerManager().SetTimer(TimerHandle, [this]()
            {
                if (this && this->IsInViewport())
                {
                    this->RemoveFromParent();
                }
            }, 3.0f, false);
    }
}
