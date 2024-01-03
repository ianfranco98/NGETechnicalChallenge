#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InGameUI.h"
#include "ScoreManager.generated.h"

UCLASS()
class NGEJERCICIOTECNICO_API AScoreManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AScoreManager();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UInGameUI* ScoreDisplay;
	TArray<uint32> FibonacciSucesion;
	TMap<FString, uint32> PlayerScore;
	FString LocalActor;

	void FindPlayers();
	void PrecalculateFibonacci(uint32 LimitTerm);
	void CalculateNewFibonacciTerms(uint32 LimitTerm);

public:
	UFUNCTION(NetMulticast, Reliable)
	void AddScoreToPlayer(const FString &WhichPlayer, int32 BlocksDestroyed);
	UFUNCTION(NetMulticast, Reliable)
	void ShowEndMatchDisplay();
};
