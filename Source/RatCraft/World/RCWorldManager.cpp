// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldManager.h"
#include "RCWorldChunk.h"
#include "RCWorldSettings.h"
#include "Blocks/RCBlock.h"
#include "Blocks/RCDataAssetBlock.h"
#include "RatCraft/Abilities/RCAbilitySystemStatics.h"
#include "RatCraft/Inventory/RCInventory.h"
#include "HAL/PlatformTime.h"
//#pragma optimize("", off)


ARCWorldManager::ARCWorldManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARCWorldManager::BeginPlay()
{
	Super::BeginPlay();

	WorldSettings = URCWorldSettings::GetSettings();
	WorldSettings->Seed = FMath::RandRange(1, MAX_int32);
	
	FRandomStream RandomStream(WorldSettings->Seed);

	FVector2D InSeedOffset = {
		RandomStream.FRandRange(1.0f, 10000.0f),
		RandomStream.FRandRange(1.0f, 10000.0f)
	};

	WorldSettings->SeedOffset = InSeedOffset;
	
	CurrentlyStandOnChunkCoords = FVector2D(-CHAR_MAX, CHAR_MIN);

	ChunkDistance = WorldSettings->RenderDistance / WorldSettings->ChunkSize;
	DoubleChunkDistance = ChunkDistance * 2;
	NewRenderSet.SetNum((DoubleChunkDistance + 1) * (DoubleChunkDistance + 1));
	OldRenderSet.SetNum((DoubleChunkDistance + 1) * (DoubleChunkDistance + 1));
	OldRenderSet.Init(FVector2D(0,0), OldRenderSet.Num());
	
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

void ARCWorldManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(UpdateWorldRenderTimerHandle);
		World->GetTimerManager().ClearTimer(BlockPlacedTimerHandle);
	}
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
	const FVector2D ChunkCoords = GetChunkCoordsFromWorldCoords(PlayerGridCoords->X, PlayerGridCoords->Y);
	if (!DidChunkChange(ChunkCoords)) return;

	//SCOPE_CYCLE_COUNTER(STAT_HandleChunkLoading);

	CurrentlyStandOnChunkCoords = ChunkCoords;
	
	for (int8 x = 0; x <= DoubleChunkDistance; x++)
	{
		for (int8 y = 0; y <= DoubleChunkDistance; y++)
		{
			const uint8 Index = x + y * (DoubleChunkDistance + 1);
			const FVector2D& Coords = FVector2D(ChunkCoords.X + ( x - ChunkDistance ), ChunkCoords.Y + ( y - ChunkDistance ));
			NewRenderSet[Index] = Coords;
			if (!AllChunks.Contains(Coords))
			{
				AddChunk(Coords.X, Coords.Y);
			}
			ARCWorldChunk* Chunk = AllChunks[Coords];
			Chunk->SetRender(true);
		}
	}
	
	for (auto It = OldRenderSet.CreateIterator(); It; ++It)
	{
		if (!NewRenderSet.Contains(*It))
		{
			AllChunks[*It]->SetRender(false);
		}
	}

	for (int8 i = 0; i < NewRenderSet.Num(); i++)
	{
		OldRenderSet[i] = NewRenderSet[i];
	}
}

bool ARCWorldManager::DidChunkChange(const FVector2D& ChunkCoords)
{
	if (CurrentlyStandOnChunkCoords == ChunkCoords)
		return false;
	return true;
}

ARCWorldChunk* ARCWorldManager::AddChunk(const int X, const int Y)
{
	const float WorldChunkSize = WorldSettings->GetWorldChunkSize();
	const FVector ChunkSpawnLocation = FVector( X * WorldChunkSize, Y * WorldChunkSize, 0);
	
	ARCWorldChunk* NewChunk = GetWorld()->SpawnActor<ARCWorldChunk>(
		ChunksClass, 
		ChunkSpawnLocation, 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	NewChunk->Init(this);
	
	AllChunks.Emplace(FVector2D(X, Y) ,NewChunk);

	return NewChunk;
}

void ARCWorldManager::Mining(const bool bIsPressed) const
{
	if (!bIsLookingAtChunk)
		return;
	
	if (bIsPressed)
	{
		if (!CurrentlyLookAtChunk->IsMining())
			CurrentlyLookAtChunk->StartMining();
	}
	else //Released
	{
		CurrentlyLookAtChunk->StopMining();
	}

	UpdateWireframe();
}

void ARCWorldManager::OnBlockMined(const EBlockType BlockType) const
{
	PlayerInventory->AddItem(BlockType);
}

bool ARCWorldManager::SpawnBlock(const EBlockType BlockType, const FVector& PlayerGridCoords, const float ColliderSize, const float ColliderHeight)
{
	if (!bCanPlaceBlock || !bIsLookingAtChunk)
		return false;

	if (BlockType == EBlockType::Air)
		return false;
	
	StartCanPlaceBlockTimer();
	
	const FVector Coords = LookAtBlockCoords + LookAtBlockNormal;
	if (!CanSpawnBlockAtGridCoords(Coords, PlayerGridCoords, ColliderSize, ColliderHeight))
		return false;
	
	FVector2D ChunkCoords = GetChunkCoordsFromWorldCoords(Coords.X, Coords.Y);

	ARCWorldChunk* FoundChunk;
	if (!AllChunks.Contains(ChunkCoords))
	{
		 FoundChunk = AddChunk(ChunkCoords.X, ChunkCoords.Y);	
	}
	else
	{
		FoundChunk = AllChunks.FindChecked(ChunkCoords);
	}

	const bool bSucceeded = FoundChunk->SpawnBlock(BlockType, Coords);

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
		if (CurrentlyLookAtChunk && CurrentlyLookAtChunk->IsMining())
		{
			CurrentlyLookAtChunk->StopMining();
		}
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

void ARCWorldManager::SetPlayerInventory(class URCInventory* InPlayerInventory)
{
	PlayerInventory = InPlayerInventory;
}

ARCWorldChunk* ARCWorldManager::GetChunkAtWorldCoords(const int X, const int Y)
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
	{
		if (CurrentlyLookAtChunk->IsMining())
			CurrentlyLookAtChunk->StopMining();
	}
	
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

UMaterialInterface* ARCWorldManager::GetMaterialFromTypeID(const uint8 BlockTypeID)
{
	if (!BlockTypeMaterials.Contains(BlockTypeID))
	{
		UMaterialInterface* Material = GetDataAssetBlockFromType(ToTEnum<EBlockType>(BlockTypeID))->GetMaterial();
		BlockTypeMaterials.Emplace(BlockTypeID, Material);
		return Material;
	}

	return BlockTypeMaterials.FindChecked(BlockTypeID);
}
