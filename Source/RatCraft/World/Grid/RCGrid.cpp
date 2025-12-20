// Fill out your copyright notice in the Description page of Project Settings.


#include "RCGrid.h"

#include "RatCraft/World/Blocks/RCBlock.h"

ARCGrid::ARCGrid()
{
}

void ARCGrid::BeginPlay()
{
	Super::BeginPlay();

	InitGrid();
}

void ARCGrid::InitGrid()
{
	for (int i = 0; i < GridWidth; i++)
	{
		for (int j = 0; j < GridDepth; j++)
		{
			for (int k = 0; k < GridHeight; k++)
			{
				const FVector Coords = FVector(i, j, k);
				FGridCell NewCell = FGridCell(FVector(i, j, k));
				Grid.Add(NewCell);

				SpawnCube(Coords);
			}
		}
	}
}

void ARCGrid::SpawnCube(const FVector& Coords)
{
	FVector WorldSpawnLocation = Coords * LengthElement;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
	ARCBlock* NewBlock = GetWorld()->SpawnActor<ARCBlock>(
		BlockClass, 
		WorldSpawnLocation, 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	NewBlock->Init(Coords);
}
