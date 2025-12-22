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
			
			for (int Y = 0; Y < GridHeight; Y++)
			{
				const FVector Coords = FVector(X, Z, Y);
				SpawnBlock(GetBlockTypeFromHeight(TerrainHeight, Y), Coords);
			}
		}
	}
}

ARCBlock* ARCGrid::SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords)
{
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

	NewBlock->Init(BlockTypeToSpawn, GridCoords);

	FGridCell NewCell = FGridCell(GridCoords, NewBlock);
	GridCells.Add(GridCoords, NewCell);

	return NewBlock;
}

FVector ARCGrid::GetGridCoordsFromWorldPosition(const FVector& WorldPosition) const
{
	return WorldPosition / LengthElement;
}

bool ARCGrid::CanSpawnBlockAtGridCoords(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float PlayerColliderSize) const
{
	return (IsPlayerObstructing(NewBlockGridCoords, PlayerGridCoords, PlayerColliderSize) && NewBlockGridCoords.Z <= GridHeight);
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

FGridCell& ARCGrid::GetGridCellFromCoords(const FVector& Coords)
{
	return GridCells.FindChecked(Coords);
}

bool ARCGrid::IsPlayerObstructing(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float PlayerColliderSize) const 
{
	const float Distance = 
	FMath::Abs(NewBlockGridCoords.X - PlayerGridCoords.X) +
	FMath::Abs(NewBlockGridCoords.Y - PlayerGridCoords.Y) +
	FMath::Abs(NewBlockGridCoords.Z - PlayerGridCoords.Z);

	UE_LOG(LogTemp, Warning, TEXT("Distance is %f"), Distance);
    
	if (Distance <= 1 + (PlayerColliderSize / LengthElement))
	{
		return false;
	}
    
	return true;
}

EBlockType ARCGrid::GetBlockTypeFromHeight(const int TerrainHeight, const int BlockHeight) const
{
	if (BlockHeight > TerrainHeight)
		return EBlockType::Air;
	else if (BlockHeight >= TerrainHeight - RockLevel && BlockHeight < SnowLevel)
		return EBlockType::Grass;
	else if (BlockHeight < TerrainHeight - RockLevel)
		return EBlockType::Stone;
	else if (BlockHeight >= SnowLevel)
		return EBlockType::Snow;
	
	return EBlockType::Air;
}
