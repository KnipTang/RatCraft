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
				GridTypeMap.Add(Coords, GetBlockTypeFromHeight(TerrainHeight, Y));
				
				const FBlockFaceVisibility BlockFaceVisibility{true, true, true, true, true, true};
				SpawnBlock(GetBlockTypeFromHeight(TerrainHeight, Y), Coords,BlockFaceVisibility);
			}
		}
	}
}

void ARCGrid::RenderGrid()
{
	for (TPair<FVector /*Coords*/, EBlockType> Element : GridTypeMap)
	{
		const bool Top		= IsBlockAtCoords(FVector(Element.Key.X, Element.Key.Y, Element.Key.Z + 1));
		const bool Bottom	= IsBlockAtCoords(FVector(Element.Key.X, Element.Key.Y, Element.Key.Z - 1));
		const bool North	= IsBlockAtCoords(FVector(Element.Key.X + 1, Element.Key.Y, Element.Key.Z));
		const bool South	= IsBlockAtCoords(FVector(Element.Key.X - 1, Element.Key.Y, Element.Key.Z));
		const bool East		= IsBlockAtCoords(FVector(Element.Key.X, Element.Key.Y + 1, Element.Key.Z));
		const bool West		= IsBlockAtCoords(FVector(Element.Key.X, Element.Key.Y - 1, Element.Key.Z));
		
		const FBlockFaceVisibility BlockFaceVisibility{Top, Bottom, North, South, East, West};
		
		SpawnBlock(Element.Value, Element.Key, BlockFaceVisibility);
	}
}

bool ARCGrid::SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords, const struct FBlockFaceVisibility BlockFaceVisibility)
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
		return false;

	NewBlock->Init(BlockDataAsset.FindChecked(BlockTypeToSpawn), BlockTypeToSpawn, GridCoords, BlockFaceVisibility);

	FGridCell NewCell = FGridCell(GridCoords, NewBlock);
	AllGridCells.Add(GridCoords, NewCell);

	return true;
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

TArray<float> ARCGrid::GeneratePerlinNoise() const
{
	return URCPerlinNoise::GenerateHeightMap(GridWidth, GridDepth, GridScale, FVector2D());
}

FGridCell& ARCGrid::GetGridCellFromCoords(const FVector& Coords)
{
	return AllGridCells.FindChecked(Coords);
}

bool ARCGrid::IsBlockAtCoords(const FVector& Coords) const
{
	if (GridTypeMap.FindChecked(Coords) == EBlockType::Air)
		return false;
	return true;
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
