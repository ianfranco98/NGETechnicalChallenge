#include "ScoreManager.h"
#include "Components/TextBlock.h"
#include "Math/UnrealMathUtility.h"
#include "Net/UnrealNetwork.h"
#include "InGameUI.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

AScoreManager::AScoreManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bNetLoadOnClient = true;
	bAlwaysRelevant = true;
}

void AScoreManager::BeginPlay()
{
	Super::BeginPlay();

	FStringClassReference MyWidgetClassRef(TEXT("/Game/UI/ScoreDisplay.ScoreDisplay_C"));

	if (UClass* MyWidgetClass = MyWidgetClassRef.TryLoadClass<UUserWidget>())
	{
		UInGameUI* NewUIInstance = CreateWidget<UInGameUI>(GetWorld(), MyWidgetClass);
		NewUIInstance->AddToViewport();
		NewUIInstance->UpdateScore(FText::FromString("Score: 0"));
		ScoreDisplay = NewUIInstance;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString("fail"));
	}
	
	PrecalculateFibonacci(20);

	SetActorTickEnabled(false);
}

void AScoreManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AScoreManager::PrecalculateFibonacci(uint32 LimitTerm)
{
	uint32 t1 = 0, t2 = 1, nextTerm = 0;
	nextTerm = t1 + t2;

	for (uint32 i = 0; i < LimitTerm; i++)
	{
		FibonacciSucesion.Add(nextTerm);
		t1 = t2;
		t2 = nextTerm;
		nextTerm = t1 + t2;
	}
}

void AScoreManager::CalculateNewFibonacciTerms(uint32 LimitTerm)
{
	if (LimitTerm > (uint32)FibonacciSucesion.Num())
	{
		uint32 t1 = FibonacciSucesion[FibonacciSucesion.Num() - 2];
		uint32 t2 = FibonacciSucesion[FibonacciSucesion.Num() - 1];

		uint32 nextTerm = t1 + t2;

		for (uint32 i = 0; i < LimitTerm; i++)
		{
			FibonacciSucesion.Add(nextTerm);
			t1 = t2;
			t2 = nextTerm;
			nextTerm = t1 + t2;
		}
	}
}

// Esta es la parte que no pude resolver. De la manera en que lo dejé
// solo actualiza el puntaje del jugador en modo server
void AScoreManager::FindPlayers()
{

	for (TActorIterator<ACharacter> It(GetWorld()); It; ++It)
	{
		FString name_ = It->GetNetOwner()->GetActorLabel();
		PlayerScore.FindOrAdd(name_, 0);

		//Métodos que fallaron:
		//
		//It->GetNetOwningPlayer()->GetPlayerController(GetWorld()) != nullptr
		//It->HasAuthority()
		//It->GetNetOwner()->HasAuthority()
		//It->HasLocalNetOwner()
		//UGameplayStatics::GetPlayerController(*It, 0)->HasAuthority()
		//UGameplayStatics::GetPlayerController(*It, 0)->IsLocalController();
		//It->GetOwner() != nullptr && It->GetOwner()->HasAuthority()
		
		bool IsServer = HasAuthority();
		bool IsClient = false;

		//FColor C = (LocalActor == FString() && (IsServer || IsClient)) ? FColor::Green : FColor::Red;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, C, name_);

		if (LocalActor == FString() && (IsServer || IsClient)) LocalActor = name_;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, TEXT("--------------"));
}

void AScoreManager::AddScoreToPlayer_Implementation(const FString &WhichPlayer, int32 BlocksDestroyed)
{
	// En realidad esto debería ejecutarse ni bien hayan cargado todos los clientes.
	if (PlayerScore.Num() == 0)
	{
		FindPlayers();
	}
	//

	if (BlocksDestroyed > FibonacciSucesion.Num())
	{
		CalculateNewFibonacciTerms(BlocksDestroyed);
	}

	uint32 TotalScoreCount = FibonacciSucesion[BlocksDestroyed - 1];

	PlayerScore[WhichPlayer] += TotalScoreCount;

	if (WhichPlayer == LocalActor) ScoreDisplay->UpdateScore(FText::FromString("Score: " + FString::FromInt(PlayerScore[WhichPlayer])));
}

void AScoreManager::ShowEndMatchDisplay_Implementation()
{
	ScoreDisplay->ShowGameOver(PlayerScore);
}
