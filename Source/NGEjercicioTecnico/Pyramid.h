#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.h"
#include "ScoreManager.h"
#include "Pyramid.generated.h"

UENUM()
enum class BlockPositionTypeInPyramid : uint8
{
	Internal ,
	LeftBorder ,
	RightBorder
};

UCLASS()
class NGEJERCICIOTECNICO_API APyramid : public AActor
{
	GENERATED_BODY()
	
public:
	APyramid();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	float BlockSize;
	float MarginBetweenBlocks;

	// Cantidad de bloques en la primera fila
	uint32 BaseSize;
	uint32 CurrentNumberOfBlocks;
	uint32 LeftCornerIndex;
	uint32 RightCornerIndex;
	uint32 TopCornerIndex;

	TArray<ABlock*> Blocks;
	TArray<FVector> BlockPositions;
	AScoreManager* ScoreManager;

	void BuildPyramid();
	TArray<ABlock*> BuildRow(FVector OriginPos, int NumBlocks, float SpacePerBlock);
	uint32 MakeFallThisBlock(uint32 BlockIndex, uint32 BlockRow, bool FallToRight);

	//El array retornado se compone de:
	//	[0] El número de fila donde está ubicado
	//	[1] Que clase de posición tiene (BlockPositionTypeInPyramid)
	TArray<uint32> GetBlockPositionInfo(uint32 BlockIndex);
	TArray<ABlock*> GetBlockNeighbors(uint32 BlockIndex, uint32 BlockRow, uint32 PosType);
	uint32 GetTotalBlocks();
	uint32 GetRowNumberOfTheBlock(uint32 BlockIndex);
	bool ThisBlockHasBase(uint32 BlockIndex, uint32 BlockRow);
};
