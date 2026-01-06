// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldManager.h"
#include "RCWorldChunck.h"
#include "RCWorldSettings.h"
#include "Blocks/RCBlock.h"
#include "PerlinNoise/RCPerlinNoise.h"

#pragma optimize("", off)

ARCWorldManager::ARCWorldManager()
{
	WorldSettings = URCWorldSettings::GetSettings();
}

void ARCWorldManager::BeginPlay()
{
	Super::BeginPlay();

	WorldSettings->Seed = FMath::RandRange(1, INT_MAX);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	WireframeBlock = GetWorld()->SpawnActor<ARCBlock>(
		WireframeBlockClass, 
		FVector(0, 0, 0), 
		FRotator::ZeroRotator, 
		SpawnParams
	);
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

bool ARCWorldManager::SpawnBlock(const FVector& Coords, const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight)
{
	if (!CanSpawnBlockAtGridCoords(Coords, PlayerGridCoords, ColliderSize, ColliderHeight))
		return false;
	
	FVector2D ChunkCoords = GetChunkCoords(Coords);

	if (!AllChunks.Contains(ChunkCoords))
	{
		AddChunck(ChunkCoords.X, ChunkCoords.Y);	
	}
	
	ARCWorldChunck* FoundChunk = AllChunks.FindChecked(ChunkCoords);

	return FoundChunk->SpawnBlock(EBlockType::Dirt, Coords, PlayerGridCoords, ColliderSize, ColliderHeight);
}

void ARCWorldManager::DisplayWireframe(const FVector& GridCoords, const FVector& LookAtBlockNormal, bool bIsLookingAtChunk)
{
	if (!bIsLookingAtChunk)
	{
		WireframeBlock->ToggleVisibility(false);
		return;
	}

	WireframeBlock->ToggleVisibility(true);
	
	const FVector WorldCoords = ((GridCoords + LookAtBlockNormal) * WorldSettings->BlockSize) + WorldSettings->GetHalfBlockSize();
	WireframeBlock->SetActorLocation(WorldCoords);
}

bool ARCWorldManager::CanSpawnBlockAtGridCoords(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight) const
{
	return (
		IsPlayerObstructing(NewBlockGridCoords, PlayerGridCoords, ColliderSize, ColliderHeight)
		&& NewBlockGridCoords.Z <= WorldSettings->ChunckHeight
		);
}

bool ARCWorldManager::IsPlayerObstructing(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, float ColliderSize, float ColliderHeight) const
{
	ColliderSize = ColliderSize / WorldSettings->BlockSize;
	ColliderHeight = ColliderHeight / WorldSettings->BlockSize;
	
	const float Distance = FMath::Sqrt(
		FMath::Square((NewBlockGridCoords.X + 0.5f) - PlayerGridCoords.X) +
		FMath::Square((NewBlockGridCoords.Y + 0.5f) - PlayerGridCoords.Y)
	);

	const float DistanceHeight = FMath::Abs((NewBlockGridCoords.Z + 0.5f) - (FMath::FloorToInt(PlayerGridCoords.Z)));
	
	return Distance >= 0.5f + ColliderSize || DistanceHeight >= 0.5f + ColliderHeight;
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
