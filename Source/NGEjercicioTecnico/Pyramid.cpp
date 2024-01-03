#include "Pyramid.h"
#include "Block.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "ScoreManager.h"
#include "Kismet/GameplayStatics.h"

APyramid::APyramid()
{
	PrimaryActorTick.bCanEverTick = true;
	UStaticMeshComponent* VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(VisualMesh);

	BaseSize = 7;
	BlockSize = 100;
	MarginBetweenBlocks = 25;

	LeftCornerIndex = 0;
	RightCornerIndex = BaseSize - 1;
	TopCornerIndex = GetTotalBlocks() - 1;
}

void APyramid::BeginPlay()
{
	Super::BeginPlay();

	ScoreManager =(AScoreManager*)UGameplayStatics::GetActorOfClass(GetWorld(), AScoreManager::StaticClass());

	if (HasAuthority())
	{
		BuildPyramid();
		CurrentNumberOfBlocks = GetTotalBlocks();
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

void APyramid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	int32 HittedBlocksCount = 0;
	bool TriggererFinded = false;
	FString TriggererProjectileOwner = FString();

	// Fase 1: Eliminación bloques
	for (int32 i = 0; i != Blocks.Num(); i++)
	{
		if (Blocks[i] != nullptr && Blocks[i]->WasHitted())
		{
			TArray<uint32> PosInfo = GetBlockPositionInfo(i);
			TArray<ABlock*> BlockNeighbors = GetBlockNeighbors(i, PosInfo[0], PosInfo[1]);

			if (!TriggererFinded)
			{
				TriggererProjectileOwner = Blocks[i]->GetProjectileOwnerLabel();
				TriggererFinded = true;
			}

			HittedBlocksCount++;

			for (auto& Block : BlockNeighbors)
			{
				if (Block != nullptr && Blocks[i]->GetColor() == Block->GetColor())
				{
					Block->Hit();
				}
			}

			//Blocks[i]->Destroy();
			Blocks[i] = nullptr;

			// Volvemos a recorrer el array desde el bloque con menor indice

			if (PosInfo[0] != 0)
			{
				uint32 numBlocksInColumn = BaseSize - PosInfo[0];

				i -= (numBlocksInColumn + 1);
			}
			else if (i > 0)
			{
				i = i - 1;
			}

			// Esta linea es para compensar el i++ del bucle for.
			// Si no estuviese, el bloque al que salta no sería
			// evaluado en este frame.
			i -= 1;
		}
	}

	// Fase 2: Reposicionar bloques

	for (uint32 i = 0; i != Blocks.Num(); i++)
	{
		if (Blocks[i] != nullptr)
		{
			// El bloque intercala de dirección comenzando
			// por la derecha
			bool FallDirection = true;

			uint32 Row = GetRowNumberOfTheBlock(i);

			while (!ThisBlockHasBase(i, Row))
			{
				i = MakeFallThisBlock(i, Row, FallDirection);
				Row = GetRowNumberOfTheBlock(i);
				FallDirection = !FallDirection;
			}
		}
	}

	// Fase 3: Comunicar los resultados con el ScoreManager

	if (HittedBlocksCount != 0)
	{
		if (ScoreManager != nullptr) ScoreManager->AddScoreToPlayer(TriggererProjectileOwner, HittedBlocksCount);
	}

	// Fase 4: Chequear si la pirámide fue destruida

	CurrentNumberOfBlocks -= HittedBlocksCount;

	if (CurrentNumberOfBlocks == 0)
	{
		ScoreManager->ShowEndMatchDisplay();
		SetActorTickEnabled(false);
	}
}


void APyramid::BuildPyramid()
{
	float InitialYPos = 0;// x
	float InitialZPos = 0;// y

	float TotalBlockSpace = BlockSize + MarginBetweenBlocks;
	float RowOffSet = TotalBlockSpace / 2 - 1;

	for (uint32 i = 0; i < BaseSize; i++)
	{
		for (auto& b : BuildRow(FVector(0, InitialYPos, InitialZPos), BaseSize - i, TotalBlockSpace))
		{
			Blocks.Add(b);
		}
		InitialZPos += TotalBlockSpace;
		InitialYPos += RowOffSet;
	}
}

TArray<ABlock*> APyramid::BuildRow(FVector OriginPos, int NumBlocks, float SpacePerBlock)
{
	TArray<ABlock*> Row;
	FVector CurrentPos = OriginPos;

	for (int i = 0; i < NumBlocks; i++)
	{
		ABlock* NewBlock = (ABlock*)GetWorld()->SpawnActor<ABlock>();
		NewBlock->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		
		NewBlock->SetInitialPosition(CurrentPos);
		Row.Add(NewBlock);
		BlockPositions.Add(CurrentPos);

		CurrentPos.Y += SpacePerBlock;
	}

	return Row;
}

uint32 APyramid::GetTotalBlocks()
{
	uint32 Total = 0;
	int BlockCountInRows = BaseSize;

	while (BlockCountInRows > 0)
	{
		Total += BlockCountInRows;
		BlockCountInRows--;
	}
	
	return Total;
}

uint32 APyramid::GetRowNumberOfTheBlock(uint32 BlockIndex)
{
	uint32 CurrentRow = 0;

	uint32 FirstRowIndex = 0;
	uint32 LastRowIndex = BaseSize - 1;
	
	//TODO: reemplazar por un while

	for (uint32 i=0; i < BaseSize; i++)
	{
		if (BlockIndex >= FirstRowIndex && BlockIndex <= LastRowIndex)
		{
			break;
		} else {
			FirstRowIndex += BaseSize - i;
			LastRowIndex += BaseSize - 1 - i;

			CurrentRow++;
		}
	}
	
	return CurrentRow;
}

//El array retornado se compone de:
//	[0] El número de fila donde está ubicado
//	[1] Que clase de posición tiene (BlockPositionTypeInPyramid)
TArray<uint32> APyramid::GetBlockPositionInfo(uint32 BlockIndex)
{
	TArray<uint32> PosInfo;

	uint32 CurrentRow = 0;
	uint32 PositionType = 0;

	uint32 FirstRowIndex = 0;
	uint32 LastRowIndex = BaseSize - 1;

	for (uint32 i = 0; i < BaseSize; i++)
	{
		if (BlockIndex >= FirstRowIndex && BlockIndex <= LastRowIndex)
		{
			if (BlockIndex == FirstRowIndex) PositionType = (uint32) BlockPositionTypeInPyramid::LeftBorder;
			else if (BlockIndex == LastRowIndex) PositionType = (uint32) BlockPositionTypeInPyramid::RightBorder;
			else PositionType = (uint32) BlockPositionTypeInPyramid::Internal;
			break;
		}
		else {
			FirstRowIndex += BaseSize - i;
			LastRowIndex += BaseSize - 1 - i;

			CurrentRow++;
		}
	}


	PosInfo.Add(CurrentRow);
	PosInfo.Add(PositionType);

	return PosInfo;
}

TArray<ABlock*> APyramid::GetBlockNeighbors(uint32 BlockIndex, uint32 BlockRow, uint32 PosType)
{
	TArray<ABlock*> BlockNeighbors;
	uint32 NumBlocksInColumn = BaseSize - BlockRow;

	// bloques contiguos
	uint32 LeftSide = BlockIndex - 1;
	uint32 RightSide = BlockIndex + 1;

	// bloques superiores
	uint32 UpperLeft = BlockIndex + (NumBlocksInColumn - 1);
	uint32 UpperRight = BlockIndex + NumBlocksInColumn;

	// bloques inferiores
	uint32 LowerLeft = BlockIndex - (NumBlocksInColumn + 1);
	uint32 LowerRight = BlockIndex - NumBlocksInColumn;

	switch (PosType)
	{
		case (uint32)BlockPositionTypeInPyramid::Internal:

			BlockNeighbors.Add(Blocks[LeftSide]);
			BlockNeighbors.Add(Blocks[RightSide]);

			BlockNeighbors.Add(Blocks[UpperLeft]);
			BlockNeighbors.Add(Blocks[UpperRight]);

			if (BlockRow != 0) // La fila 0 no tiene inferiores
			{
				BlockNeighbors.Add(Blocks[LowerLeft]);
				BlockNeighbors.Add(Blocks[LowerRight]);
			}
			break;
		case (uint32)BlockPositionTypeInPyramid::LeftBorder:

			// Este caso especial aparece acá debido a la forma en la que
			// está armada GetBlockPositionInfo ("if (BlockIndex == FirstRowIndex)" está primero);
			if (BlockIndex == TopCornerIndex)
			{
				BlockNeighbors.Add(Blocks[LowerLeft]);
				BlockNeighbors.Add(Blocks[LowerRight]);
			} else {

				BlockNeighbors.Add(Blocks[UpperRight]);
				BlockNeighbors.Add(Blocks[RightSide]);

				if (BlockIndex != LeftCornerIndex)
				{
					BlockNeighbors.Add(Blocks[LowerLeft]);
					BlockNeighbors.Add(Blocks[LowerRight]);
				}
			}
			break;
		case (uint32)BlockPositionTypeInPyramid::RightBorder:

			BlockNeighbors.Add(Blocks[LeftSide]);
			BlockNeighbors.Add(Blocks[UpperLeft]);

			if (BlockIndex != RightCornerIndex)
			{
				BlockNeighbors.Add(Blocks[LowerLeft]);
				BlockNeighbors.Add(Blocks[LowerRight]);
			}

			break;
	}

	return BlockNeighbors;
}

bool APyramid::ThisBlockHasBase(uint32 BlockIndex, uint32 BlockRow)
{
	uint32 numBlocksInColumn = BaseSize - BlockRow;

	uint32 LowerLeft = BlockIndex - (numBlocksInColumn + 1);
	uint32 LowerRight = BlockIndex - numBlocksInColumn;

	bool Checking = BlockRow == 0 ? true : (Blocks[LowerLeft] != nullptr) && (Blocks[LowerRight] != nullptr);

	return Checking;
}

uint32 APyramid::MakeFallThisBlock(uint32 BlockIndex, uint32 BlockRow, bool FallToRight)
{
	uint32 NumBlocksInColumn = BaseSize - BlockRow;
	uint32 NewBlockIndex = BlockIndex;

	uint32 LowerLeft = BlockIndex - (NumBlocksInColumn + 1);
	uint32 LowerRight = BlockIndex - NumBlocksInColumn;

	bool HoleOnTheLeft = Blocks[LowerLeft] == nullptr;
	bool HoleOnTheRight = Blocks[LowerRight] == nullptr;

	if (FallToRight)
	{
		if (HoleOnTheRight)
		{
			Blocks[LowerRight] = Blocks[BlockIndex];
			NewBlockIndex = LowerRight;
		}
		else if (HoleOnTheLeft)
		{
			Blocks[LowerLeft] = Blocks[BlockIndex];
			NewBlockIndex = LowerLeft;
		}
	}
	else
	{
		if (HoleOnTheLeft)
		{
			Blocks[LowerLeft] = Blocks[BlockIndex];
			NewBlockIndex = LowerLeft;
		}
		else if (HoleOnTheRight)
		{
			Blocks[LowerRight] = Blocks[BlockIndex];
			NewBlockIndex = LowerRight;
		}
	}

	Blocks[BlockIndex] = nullptr;
	Blocks[NewBlockIndex]->SetPosition(BlockPositions[NewBlockIndex]);

	return NewBlockIndex;
}
