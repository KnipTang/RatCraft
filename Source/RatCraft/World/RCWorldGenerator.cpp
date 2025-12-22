// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldGenerator.h"

#include "Blocks/RCBlock.h"
#include "Grid/RCGrid.h"
#include "PerlinNoise/RCPerlinNoise.h"

void ARCWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	CreateGrid();
}

void ARCWorldGenerator::CreateGrid()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	Grid = GetWorld()->SpawnActor<ARCGrid>(
		GridClass, 
		FVector(0, 0, 0), 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	Grid->InitGrid();
}