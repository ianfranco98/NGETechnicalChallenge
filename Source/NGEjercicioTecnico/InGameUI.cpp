#include "InGameUI.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	GameOverLabel->SetVisibility(ESlateVisibility::Hidden);

	ScoreboardLines.Add(Scoreboard);
	ScoreboardLines.Add(Scoreboard_1);
	ScoreboardLines.Add(Scoreboard_2);
	ScoreboardLines.Add(Scoreboard_3);

	for (auto& Elem : ScoreboardLines)
	{
		Elem->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInGameUI::UpdateScore(FText newScore)
{
	ScoreDisplay->SetText(newScore);
}

void UInGameUI::ShowGameOver(TMap<FString, uint32> Scores)
{
	FTimerDelegate BindDel;
	FTimerHandle TimeHandler;
	BindDel.BindUFunction(this, FName("UpdateAndShowScoreboard"), Scores);
	
	ScoreDisplay->SetVisibility(ESlateVisibility::Hidden);
	GameOverLabel->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(TimeHandler, BindDel, 3.f, false);
}

void UInGameUI::UpdateAndShowScoreboard(TMap<FString, uint32> Scores)
{
	ScoreDisplay->SetVisibility(ESlateVisibility::Hidden);
	GameOverLabel->SetVisibility(ESlateVisibility::Hidden);

	FText TextScore = FText::FromString("");

	int i = 0;
	for (auto& Elem : Scores)
	{
		FText FinalText = FText::FromString((FString)Elem.Key + " Score: " + FString::FromInt(Elem.Value));
		ScoreboardLines[i]->SetText(FinalText);
		ScoreboardLines[i]->SetVisibility(ESlateVisibility::Visible);
		i++;
	}
}