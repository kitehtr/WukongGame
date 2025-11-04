#include "Project/WukongGame/Public/UI/UWB_ComboWidget.h"
#include "Components/TextBlock.h"
#include "Animation/UMGSequencePlayer.h"

void UUWB_ComboWidget::UpdateComboDisplay(int32 ComboCount, float ComboScore, const FString& Rank, float Multiplier)
{
    if (ComboCountText)
    {
        ComboCountText->SetText(FText::FromString(FString::Printf(TEXT("%d X"), ComboCount)));
        ComboCountText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
    }

    if (ComboScoreText)
    {
        ComboScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %.0f"), ComboScore)));
        ComboScoreText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
    }

    if (ComboRankText)
    {
        ComboRankText->SetText(FText::FromString(Rank));

        FLinearColor RankColor = FLinearColor::White;
        if (Rank == "S")
            RankColor = FLinearColor(1.0f, 0.84f, 0.0f);
        else if (Rank == "A")
            RankColor = FLinearColor(0.0f, 1.0f, 0.0f);
        else if (Rank == "B")
            RankColor = FLinearColor(0.0f, 0.5f, 1.0f); 
        else if (Rank == "C")
            RankColor = FLinearColor(0.8f, 0.8f, 0.8f);
        else
            RankColor = FLinearColor(0.5f, 0.5f, 0.5f); 

        ComboRankText->SetColorAndOpacity(FSlateColor(RankColor));
    }

    if (ComboMultiplierText)
    {
        ComboMultiplierText->SetText(FText::FromString(FString::Printf(TEXT("x%.1f"), Multiplier)));
        ComboMultiplierText->SetColorAndOpacity(FLinearColor(0.0f, 0.5f, 1.0f));
    }

    ShowComboWidget();
}

void UUWB_ComboWidget::ShowComboWidget()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UUWB_ComboWidget::HideComboWidget()
{
    SetVisibility(ESlateVisibility::Hidden);
}

void UUWB_ComboWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetVisibility(ESlateVisibility::Hidden);
}
