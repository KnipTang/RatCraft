// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RatCraft/World/Blocks/RCBlockTypes.h"
#include "RCGrid.generated.h"

/**
 * 
 */
USTRUCT()
struct FGridCell
{
	GENERATED_BODY()
public:
	FGridCell() = default;
	FGridCell(const FVector& InCoords, class ARCBlock* InBlock) : 
		Coordinates(InCoords),
		Block(InBlock)
	{}
	
	UPROPERTY(EditAnywhere)
	FVector Coordinates = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	class ARCBlock* Block;
};

UCLASS()
class RATCRAFT_API ARCGrid : public AActor
{
	GENERATED_BODY()

public:
	ARCGrid();

	virtual void BeginPlay() override;
	int GetElementLength() const { return LengthElement; }

	void InitGrid();
	ARCBlock* SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords);

	FVector GetGridCoordsFromWorldPosition(const FVector& WorldPosition) const;

	bool CanSpawnBlockAtGridCoords(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords) const;
private:
	float GetNoiseHeightAt(int X, int Z);
	
	TArray<float> GeneratePerlinNoise();
	
	FGridCell& GetGridCellFromCoords(const FVector& Coords);

	TMap<FVector /*Coords*/, FGridCell> GridCells;
	
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int LengthElement;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int GridWidth;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int GridDepth;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int GridHeight;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float GridScale = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Blocks")
	TSubclassOf<class ARCBlock> BlockClass;

	TArray<float> PerlinNoise;
};
