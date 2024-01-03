#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.generated.h"

UCLASS()
class NGEJERCICIOTECNICO_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* ScoreDisplay = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* GameOverLabel = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* Scoreboard = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* Scoreboard_1 = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* Scoreboard_2 = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* Scoreboard_3 = nullptr;
	
	TArray<UTextBlock*> ScoreboardLines;

	virtual void NativeConstruct() override;
	void UpdateScore(FText newScore);
	void ShowGameOver(TMap<FString, uint32> Scores);
	UFUNCTION()
	void UpdateAndShowScoreboard(TMap<FString, uint32> Scores);
	
};
