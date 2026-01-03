// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldManager.h"
#include "RCWorldChunck.h"
#include "RCWorldSettings.h"

#pragma optimize("", off)

ARCWorldManager::ARCWorldManager()
{
	WorldSettings = URCWorldSettings::GetSettings();
}

void ARCWorldManager::BeginPlay()
{
	Super::BeginPlay();

	AddChunck(0, 0);
	AddChunck(1, 0);
	AddChunck(2, 0);
		AddChunck(2, 1);
}

void ARCWorldManager::AddChunck(int X, int Y)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector ChunckSpawnLocation = FVector( X * WorldSettings->GetWorldChunckSize(), Y * WorldSettings->GetWorldChunckSize(), 0);
	
	ARCWorldChunck* NewChunk = GetWorld()->SpawnActor<ARCWorldChunck>(
		ChunksClass, 
		ChunckSpawnLocation, 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	AllChunks.Emplace(FVector2D(X, Y) ,NewChunk);
}

bool ARCWorldManager::SpawnBlock(FVector& Coords)
{
	int CoordsX = static_cast<int>(Coords.X);
	int CoordsY = static_cast<int>(Coords.Y);
	
	int ChunkCoordsX = CoordsX / (WorldSettings->ChunckSize);
	int ChunkCoordsY = CoordsY / (WorldSettings->ChunckSize);
	FVector2D ChunkCoords = FVector2D(ChunkCoordsX, ChunkCoordsY);
	
	FVector SpawnCoord = FVector(
		CoordsX % (WorldSettings->ChunckSize),
		CoordsY % (WorldSettings->ChunckSize),
		Coords.Z);

	if (!AllChunks.Contains(ChunkCoords))
	{
		AddChunck(ChunkCoordsX, ChunkCoordsY);	
	}
	
	ARCWorldChunck* FoundChunk = AllChunks.FindChecked(ChunkCoords);
	
	return FoundChunk->SpawnBlock(EBlockType::Dirt, Coords);
}
