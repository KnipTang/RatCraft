// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldManager.h"
#include "RCWorldChunk.h"
#include "RCWorldSettings.h"
#include "Blocks/RCBlock.h"
#include "RatCraft/Abilities/RCAbilitySystemStatics.h"

ARCWorldManager::ARCWorldManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARCWorldManager::BeginPlay()
{
	Super::BeginPlay();

	WorldSettings = URCWorldSettings::GetSettings();
	WorldSettings->Seed = FMath::RandRange(1, INT_MAX);
	
	for (int8 x = -WorldSettings->InitChunksLoadedRange; x < WorldSettings->InitChunksLoadedRange; x++)
	{
		for (int8 y = -WorldSettings->InitChunksLoadedRange; y < WorldSettings->InitChunksLoadedRange; y++)
		{
			AddChunk(x,y);
		}
	}
	
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	WireframeBlock = GetWorld()->SpawnActor<ARCBlock>(
		WireframeBlockClass, 
		FVector(0, 0, 0), 
		FRotator::ZeroRotator, 
		SpawnParams
	);
}

void ARCWorldManager::EnableChunkLoading(const FVector* PlayerGridCoords)
{
	if (!PlayerGridCoords)
		return;
	
	if (UWorld* World = GetWorld())
		World->GetTimerManager().SetTimer(
		UpdateWorldRenderTimerHandle,
		[this, PlayerGridCoords]()
		 {
			if (!PlayerGridCoords)
				return;
			HandleChunkLoading(PlayerGridCoords);
		 },
		UpdateWorldRenderCooldown,
		true
	);
}

void ARCWorldManager::HandleChunkLoading(const FVector* PlayerGridCoords)
{
	for (ARCWorldChunk* Chunk : RenderedChunks)
	{
		Chunk->SetRender(false);
	}

	RenderedChunks.Empty();
	const FVector2D ChunkCoords = GetChunkCoordsFromWorldCoords(PlayerGridCoords->X, PlayerGridCoords->Y);

	int ChunkDistance = WorldSettings->RenderDistance / WorldSettings->ChunkSize;
	ChunkDistance++;
	
	for (int x = -ChunkDistance; x <= ChunkDistance; x++)
	{
		for (int y = -ChunkDistance; y <= ChunkDistance; y++)
		{
			RenderChunk(FVector2D(ChunkCoords.X + x, ChunkCoords.Y + y));
		}
	}
}

void ARCWorldManager::RenderChunk(const FVector2D& Coords)
{
	if (!AllChunks.Contains(Coords))
	{
		AddChunk(Coords.X, Coords.Y);
	}

	ARCWorldChunk* RenderedChunk = AllChunks.FindChecked(Coords);

	if (!RenderedChunks.Contains(RenderedChunk))
	{
		RenderedChunks.Emplace(RenderedChunk);
	}
	
	RenderedChunk->SetRender(true);
}

void ARCWorldManager::AddChunk(int X, int Y)
{
	const FVector ChunkSpawnLocation = FVector( X * WorldSettings->GetWorldChunkSize(), Y * WorldSettings->GetWorldChunkSize(), 0);
	
	ARCWorldChunk* NewChunk = GetWorld()->SpawnActor<ARCWorldChunk>(
		ChunksClass, 
		ChunkSpawnLocation, 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	NewChunk->Init(this);
	
	AllChunks.Emplace(FVector2D(X, Y) ,NewChunk);
}

void ARCWorldManager::Mining(const bool bIsPressed)
{
	if (!bIsLookingAtChunk)
		return;
	
	if (bIsPressed)
	{
		if (!CurrentlyLookAtChunk->IsMining())
			CurrentlyLookAtChunk->OnInteract();
	}
	else //Released
	{
		CurrentlyLookAtChunk->EndInteract();
	}
	
	UpdateWireframe();
}

bool ARCWorldManager::SpawnBlock(const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight)
{
	
	if (!bCanPlaceBlock)
		return false;
	
	StartCanPlaceBlockTimer();
	
	const FVector Coords = LookAtBlockCoords + LookAtBlockNormal;
	if (!CanSpawnBlockAtGridCoords(Coords, PlayerGridCoords, ColliderSize, ColliderHeight))
		return false;
	
	FVector2D ChunkCoords = GetChunkCoordsFromWorldCoords(Coords.X, Coords.Y);

	if (!AllChunks.Contains(ChunkCoords))
	{
		AddChunk(ChunkCoords.X, ChunkCoords.Y);	
	}
	
	ARCWorldChunk* FoundChunk = AllChunks.FindChecked(ChunkCoords);

	const bool bSucceeded = FoundChunk->SpawnBlock(EBlockType::Dirt, Coords);

	if (!bSucceeded)
		return false;
	
	UpdateWireframe();
	return true;
}

void ARCWorldManager::UpdateWireframe() const
{
	if (!bIsLookingAtChunk)
	{
		WireframeBlock->ToggleVisibility(false);
		return;
	}

	WireframeBlock->ToggleVisibility(true);
	
	const FVector WorldCoords = (LookAtBlockCoords + LookAtBlockNormal) * WorldSettings->BlockSize + WorldSettings->GetHalfBlockSize();
	WireframeBlock->SetActorLocation(WorldCoords);
}

void ARCWorldManager::UpdateInteractableChunk(const float InteractDistance, const FVector& ViewCamLocation, const FRotator& ViewCamRotation)
{
	const FHitResult HitResult = URCAbilitySystemStatics::GetHitscanTarget(
		GetWorld(),
		ViewCamLocation,
		ViewCamRotation,
		ECC_WorldStatic,
		InteractDistance
		);

	if (!HitResult.GetActor())
	{
		bIsLookingAtChunk = false;
		return;
	}

	if (HitResult.GetActor() != CurrentlyLookAtChunk)
	{
		if (ARCWorldChunk* InteractedChunk = Cast<ARCWorldChunk>(HitResult.GetActor()))
		{
			LookAtChunkChanged(InteractedChunk);
		}
		else
		{
			bIsLookingAtChunk = false;
			return;
		}
	}
	
	bIsLookingAtChunk = true;
	
	LookAtBlockNormal = HitResult.ImpactNormal;
	LookAtBlockCoords = HitResult.Location / WorldSettings->BlockSize -
		FVector(
		FMath::Clamp(LookAtBlockNormal.X, 0, 1),
		FMath::Clamp(LookAtBlockNormal.Y, 0, 1),
		FMath::Clamp(LookAtBlockNormal.Z, 0, 1));
	
	constexpr float SnapEpsilon = 0.0001f;
	LookAtBlockCoords = FVector(
		FMath::Floor(LookAtBlockCoords.X + SnapEpsilon),
		FMath::Floor(LookAtBlockCoords.Y + SnapEpsilon),
		FMath::Floor(LookAtBlockCoords.Z + SnapEpsilon)
	);
	
	CurrentlyLookAtChunk->SetCurrentlyLookAtBlock(LookAtBlockCoords);
}

class ARCWorldChunk* ARCWorldManager::GetChunkAtWorldCoords(const int X, const int Y)
{
	FVector2D ChunkCoords = GetChunkCoordsFromWorldCoords(X, Y);
	
	if (!AllChunks.Contains(ChunkCoords))
	{
		return nullptr;
	}

	return AllChunks.FindChecked(ChunkCoords);
}

void ARCWorldManager::LookAtChunkChanged(ARCWorldChunk* NewChunk)
{
	if (CurrentlyLookAtChunk)
		CurrentlyLookAtChunk->EndInteract();
	
	CurrentlyLookAtChunk = NewChunk;
}

void ARCWorldManager::StartCanPlaceBlockTimer()
{
	bCanPlaceBlock = false;
	
	if (UWorld* World = GetWorld())
		World->GetTimerManager().SetTimer(
		BlockPlacedTimerHandle,
		[this]()
		 {
			 bCanPlaceBlock = true;
		 },
		BlockPlacedCooldown,
		false
	);
}

bool ARCWorldManager::CanSpawnBlockAtGridCoords(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight) const
{
	return (
		IsPlayerObstructing(NewBlockGridCoords, PlayerGridCoords, ColliderSize, ColliderHeight)
		&& NewBlockGridCoords.Z <= WorldSettings->ChunkHeight
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

	const float DistanceHeight = FMath::Abs((NewBlockGridCoords.Z + 0.5f) - (FMath::RoundToInt(PlayerGridCoords.Z)));
	
	return Distance >= 0.5f + ColliderSize || DistanceHeight >= 0.5f + ColliderHeight;
}

FVector2D ARCWorldManager::GetChunkCoordsFromWorldCoords(int X, int Y) const
{
	if (X < 0) X -= WorldSettings->ChunkSize - 1;
	if (Y < 0) Y -= WorldSettings->ChunkSize - 1;

	const int ChunkCoordsX = X / WorldSettings->ChunkSize;
	const int ChunkCoordsY = Y / WorldSettings->ChunkSize;
	
	return FVector2D(ChunkCoordsX, ChunkCoordsY);
}

URCDataAssetBlock* ARCWorldManager::GetDataAssetBlockFromType(const EBlockType BlockType) const
{
	if (BlockDataAsset.Contains(BlockType))
		return BlockDataAsset.FindChecked(BlockType);
	return nullptr;
}
