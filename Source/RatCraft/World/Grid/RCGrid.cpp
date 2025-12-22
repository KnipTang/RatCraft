// Fill out your copyright notice in the Description page of Project Settings.


#include "RCGrid.h"

#include "RatCraft/World/Blocks/RCBlock.h"
#include "RatCraft/World/PerlinNoise/RCPerlinNoise.h"

ARCGrid::ARCGrid()
{
}

void ARCGrid::BeginPlay()
{
	Super::BeginPlay();
}

void ARCGrid::InitGrid()
{
	PerlinNoise = GeneratePerlinNoise();
	
	for (int X = 0; X < GridWidth; X++)
	{
		for (int Z = 0; Z < GridDepth; Z++)
		{
			float NoiseHeight = GetNoiseHeightAt(X, Z);
			int TerrainHeight = FMath::RoundToInt(NoiseHeight * GridHeight);
			TerrainHeight = FMath::Clamp(TerrainHeight, 1, GridHeight - 1);
			UE_LOG(LogTemp, Warning, TEXT("TerreinHEight: %d"), 
			TerrainHeight);
			
			for (int Y = 0; Y < TerrainHeight; Y++)
			{
				
				const FVector Coords = FVector(X, Z, Y);
				ARCBlock* NewBlock = SpawnBlock(EBlockType::Grass, Coords);
				FGridCell NewCell = FGridCell(Coords, NewBlock);
				GridCells.Add(Coords, NewCell);
			}
		}
	}
}

ARCBlock* ARCGrid::SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords)
{
	const TSubclassOf<ARCBlock> BlockClass = GetBlockClassByType(BlockTypeToSpawn);
	
	const FVector WorldSpawnLocation = GridCoords * LengthElement;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
	ARCBlock* NewBlock = GetWorld()->SpawnActor<ARCBlock>(
		BlockClass, 
		WorldSpawnLocation, 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	if (!NewBlock)
		return nullptr;

	NewBlock->Init(GridCoords);

	return NewBlock;
}

float ARCGrid::GetNoiseHeightAt(int X, int Z)
{
	if (PerlinNoise.Num() == 0)
		return 0.0f;
	
	int NoiseIndex = X + Z * GridWidth;
    
	if (NoiseIndex >= 0 && NoiseIndex < PerlinNoise.Num())
	{
		return PerlinNoise[NoiseIndex];
	}
    
	return 0.0f;
}

TArray<float> ARCGrid::GeneratePerlinNoise()
{
	return URCPerlinNoise::GenerateHeightMap(GridWidth, GridDepth, GridScale, FVector2D());
}

TSubclassOf<class ARCBlock> ARCGrid::GetBlockClassByType(const EBlockType BlockType)
{
	return BlockClasses.FindChecked(BlockType);
}

FGridCell& ARCGrid::GetGridCellFromCoords(const FVector& Coords)
{
	return GridCells.FindChecked(Coords);
}