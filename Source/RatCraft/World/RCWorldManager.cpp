// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldManager.h"
#include "RCWorldChunck.h"
#include "RCWorldSettings.h"
#include "Blocks/RCBlock.h"
#include "PerlinNoise/RCPerlinNoise.h"
#include "RatCraft/Abilities/RCAbilitySystemStatics.h"

#pragma optimize("", off)

ARCWorldManager::ARCWorldManager()
{

}

void ARCWorldManager::BeginPlay()
{
	Super::BeginPlay();

	WorldSettings = URCWorldSettings::GetSettings();
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
			HandleChunckLoading(PlayerGridCoords);
		 },
		UpdateWorldRenderCooldown,
		true
	);
}

void ARCWorldManager::HandleChunckLoading(const FVector* PlayerGridCoords)
{
	for (ARCWorldChunck* Chunck : RenderedChunks)
	{
		Chunck->SetRender(false);
	}

	RenderedChunks.Empty();
	const FVector2D ChunkCoords = GetChunkCoords(*PlayerGridCoords);

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

void ARCWorldManager::Mining(const bool bIsPressed)
{
	if (!bIsLookingAtChunk)
		return;
	
	if (bIsPressed)
	{
		if (!CurrentlyLookAtChunck->IsMining())
			CurrentlyLookAtChunck->OnInteract();
	}
	else //Released
	{
		CurrentlyLookAtChunck->EndInteract();
	}
	
	UpdateWireframe();
}

bool ARCWorldManager::SpawnBlock(const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight)
{
	if (!bCanPlaceBlock)
		return false;
	
	const FVector Coords = LookAtBlockCoords + LookAtBlockNormal;
	if (!CanSpawnBlockAtGridCoords(Coords, PlayerGridCoords, ColliderSize, ColliderHeight))
		return false;
	
	FVector2D ChunkCoords = GetChunkCoords(Coords);

	if (!AllChunks.Contains(ChunkCoords))
	{
		AddChunck(ChunkCoords.X, ChunkCoords.Y);	
	}
	
	ARCWorldChunck* FoundChunk = AllChunks.FindChecked(ChunkCoords);

	const bool bSucceeded = FoundChunk->SpawnBlock(EBlockType::Dirt, Coords, PlayerGridCoords, ColliderSize, ColliderHeight);

	if (!bSucceeded)
		return false;

	StartCanPlaceBlockTimer();
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

void ARCWorldManager::UpdateInteractableChunck(const float InteractDistance, const FVector& ViewCamLocation, const FRotator& ViewCamRotation)
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

	if (HitResult.GetActor() != CurrentlyLookAtChunck)
	{
		if (ARCWorldChunck* InteractedChunck = Cast<ARCWorldChunck>(HitResult.GetActor()))
		{
			LookAtChunckChanged(InteractedChunck);
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
	
	CurrentlyLookAtChunck->SetCurrentlyLookAtBlock(LookAtBlockCoords);
}

void ARCWorldManager::LookAtChunckChanged(ARCWorldChunck* NewChunck)
{
	if (CurrentlyLookAtChunck)
		CurrentlyLookAtChunck->EndInteract();
	
	CurrentlyLookAtChunck = NewChunck;
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

FVector2D ARCWorldManager::GetChunkCoords(const FVector& WorldCoords) const
{
	int CoordsX = static_cast<int>(WorldCoords.X);
	int CoordsY = static_cast<int>(WorldCoords.Y);

	if (CoordsX < 0) CoordsX -= WorldSettings->ChunckSize - 1;
	if (CoordsY < 0) CoordsY -= WorldSettings->ChunckSize - 1;

	const int ChunkCoordsX = CoordsX / WorldSettings->ChunckSize;
	const int ChunkCoordsY = CoordsY / WorldSettings->ChunckSize;
	
	return FVector2D(ChunkCoordsX, ChunkCoordsY);
}
