// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RCGrid.generated.h"

/**
 * 
 */
USTRUCT()
struct FGridCell
{
	GENERATED_BODY()
	FGridCell() = default;
	FGridCell(const FVector& Coords) : 
		Coordinates(Coords)
	{}

	UPROPERTY(EditAnywhere)
	FVector Coordinates = FVector::ZeroVector;
};

UCLASS()
class RATCRAFT_API ARCGrid : public AActor
{
	GENERATED_BODY()

public:
	ARCGrid();

	virtual void BeginPlay() override;
	int GetElementLength() const { return LengthElement; }
	
	void SpawnCube(const FVector& Coords);
private:
	void InitGrid();

	
	TArray<FGridCell> Grid;
	
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int LengthElement;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int GridWidth;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int GridDepth;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int GridHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	TSubclassOf<class AActor> BlockClass;
};
