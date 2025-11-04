#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UWB_ComboWidget.generated.h"

/**
 * 
 */
UCLASS()
class WUKONG_API UUWB_ComboWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* ComboCountText;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* ComboScoreText;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* ComboRankText;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* ComboMultiplierText;

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void UpdateComboDisplay(int32 ComboCount, float ComboScore, const FString& Rank, float Multiplier);

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void ShowComboWidget();

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void HideComboWidget();

    /*UFUNCTION(BlueprintCallable, Category = "Combo")
    void PlayComboAnimation(bool bIsStyleBonus = false);*/

protected:
    virtual void NativeConstruct() override;
};
