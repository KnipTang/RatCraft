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
}

void ARCWorldManager::HandleChunckLoading(const FVector& PlayerCoords)
{
	for (class ARCWorldChunck* Chunck : RenderedChunks)
	{
		Chunck->SetRender(false);
	}

	RenderedChunks.Empty();
	const FVector2D ChunkCoords = GetChunkCoords(PlayerCoords);

	int ChunkDistance = WorldSettings->RenderDistance / WorldSettings->ChunckSize;
	ChunkDistance++;
	
	for (int x = -ChunkDistance; x <= ChunkDistance; x++)
	{
		for (int y = -ChunkDistance; y <= ChunkDistance; y++)
		{
			RenderChunck(FVector2D(ChunkCoords.X + x, ChunkCoords.Y + y));
		}
	}
}

void ARCWorldManager::RenderChunck(const FVector2D& Coords)
{
	if (!AllChunks.Contains(Coords))
	{
		AddChunck(Coords.X, Coords.Y);
	}

	ARCWorldChunck* RenderedChunk = AllChunks.FindChecked(Coords);

	if (!RenderedChunks.Contains(RenderedChunk))
	{
		RenderedChunks.Emplace(RenderedChunk);
	}
	
	RenderedChunk->SetRender(true);
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
	NewChunk->SetRender(false);
}

bool ARCWorldManager::SpawnBlock(FVector& Coords)
{
	FVector2D ChunkCoords = GetChunkCoords(Coords);

	if (!AllChunks.Contains(ChunkCoords))
	{
		AddChunck(ChunkCoords.X, ChunkCoords.Y);	
	}
	
	ARCWorldChunck* FoundChunk = AllChunks.FindChecked(ChunkCoords);

	return FoundChunk->SpawnBlock(EBlockType::Dirt, Coords);
}

FVector2D ARCWorldManager::GetChunkCoords(const FVector& WorldCoords)
{
	int CoordsX = static_cast<int>(WorldCoords.X);
	int CoordsY = static_cast<int>(WorldCoords.Y);

	if (CoordsX < 0) CoordsX -= WorldSettings->ChunckSize - 1;
	if (CoordsY < 0) CoordsY -= WorldSettings->ChunckSize - 1;
	
	int ChunkCoordsX = CoordsX / WorldSettings->ChunckSize;
	int ChunkCoordsY = CoordsY / WorldSettings->ChunckSize;
	
	return FVector2D(ChunkCoordsX, ChunkCoordsY);
}
