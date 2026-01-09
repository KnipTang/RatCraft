// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldChunk.h"
#include "ProceduralMeshComponent.h"
#include "RCWorldManager.h"
#include "RCWorldSettings.h"
#include "Blocks/RCBlockStatics.h"
#include "Blocks/RCDataAssetBlock.h"
#include "PerlinNoise/RCPerlinNoise.h"
//#pragma optimize("", off)
// Sets default values
ARCWorldChunk::ARCWorldChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->SetCollisionResponseToAllChannels(ECR_Block);
	RootComponent = ProceduralMesh;
}

void ARCWorldChunk::Init(ARCWorldManager* InWorldManager)
{
	WorldManager = InWorldManager;
	WorldSettings = URCWorldSettings::GetSettings();

	ChunkSize = WorldSettings->ChunkSize;
	ChunkSizeWithBorder = WorldSettings->ChunkSize + 2;
	ChunkHeight = WorldSettings->ChunkHeight;
	TotalBlocks = ChunkSize * ChunkSize * ChunkHeight;
	BlockSize = WorldSettings->BlockSize;
	HalfBlockSize = WorldSettings->GetHalfBlockSize();
	RockLevel = WorldSettings->RockLevel;
	SnowLevel = WorldSettings->SnowLevel;

	SetRender(false);

	ChunkWorldCoords = GetActorLocation() / BlockSize;
	ChunkGridCoords = FVector2D( ChunkWorldCoords / ChunkSize);

	for (uint8 i = 0; i < BlockTypesCount; i++)
	{
		if (WorldManager)
		{
			UMaterialInterface* Material = WorldManager->GetMaterialFromTypeID(i);
			ProceduralMesh->SetMaterial(i, Material);
		}
	}

	PerlinNoise = GeneratePerlinNoise();
	InitChunkBlockData();
	RenderChunk();
}

void ARCWorldChunk::SetRender(const bool bRender)
{
	if (bIsRendered == bRender)
		return;

	bIsRendered = bRender;
	
	ProceduralMesh->SetVisibility(bRender);
	SetActorHiddenInGame(!bRender);
	SetCollision(bRender);
}

void ARCWorldChunk::SetCollision(const bool bCollision)
{
	if (bIsCollision == bCollision)
		return;

	bIsCollision = bCollision;
	
	ProceduralMesh->SetActive(bCollision);
	ProceduralMesh->SetComponentTickEnabled(bCollision);
	ProceduralMesh->SetCollisionEnabled(bCollision ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	SetActorEnableCollision(bCollision);
}

void ARCWorldChunk::InitChunkBlockData()
{
	const uint32 TotalBlocksWithBorder = ChunkSizeWithBorder * ChunkSizeWithBorder * ChunkHeight;
	
	ChunkBlocksData.Empty(TotalBlocksWithBorder);
	ChunkBlocksData.SetNum(TotalBlocksWithBorder);
	
	for (int8 X = -1; X < ChunkSize + 1; X++)
	{
		for (int8 Z = -1; Z < ChunkSize + 1; Z++)
		{
			const uint8 TerrainHeight = GetNoiseHeightAt(X + 1, Z + 1);
			for (uint8 Y = 0; Y < ChunkHeight; Y++)
			{
				const EBlockType BlockType = GetBlockTypeFromHeight(TerrainHeight, Y);
				const int32 Index = GetBlockIndex(X, Z, Y);
                ChunkBlocksData[Index] = BlockType;
			}
		}
	}
}

void ARCWorldChunk::RenderChunk()
{
	ChunkMeshes = {};
	ChunkMeshes.SetNum(BlockTypesCount);
	
	for (int32 X = 0; X < ChunkSize; X++)
	{
		for (int32 Y = 0; Y < ChunkSize; Y++)
		{
			for (int32 Z = 0; Z < ChunkHeight; Z++)
			{
				const int32 Index = GetBlockIndex(X, Y, Z);
				if (Index >= ChunkBlocksData.Num())
					continue;
                    
				EBlockType BlockType = ChunkBlocksData[Index];
				if (BlockType == EBlockType::Air)
					continue;
                
				GenerateBlockFaces(BlockType, FVector(X, Y, Z));
			}
		}
	}
	
	CreateProceduralMesh();
}

void ARCWorldChunk::GenerateBlockFaces(const EBlockType& BlockType, const FVector& BlockCoords)
{
	const uint8 FaceBitmask = GetBlockFaceBitmaskVisibility(BlockCoords.X, BlockCoords.Y, BlockCoords.Z);
	if (FaceBitmask == 0)
		return;
	
	const uint8 MaterialIndex = ToUInt8(BlockType);
	
	TArray<FVector>& Vertices = ChunkMeshes[MaterialIndex].Vertices;
	TArray<int32>& Triangles = ChunkMeshes[MaterialIndex].Triangles;
	TArray<FVector>& Normals = ChunkMeshes[MaterialIndex].Normals;
	TArray<FVector2D>& UVs = ChunkMeshes[MaterialIndex].UVs;
	TArray<FColor>& VertexColors = ChunkMeshes[MaterialIndex].VertexColors;

	for (uint8 FaceIndex = 0; FaceIndex < Faces.Num(); FaceIndex++)
	{
		if (!(FaceBitmask & (1 << FaceIndex)))
			continue;
		
		const TArray<uint8>& Face = Faces[FaceIndex];
		uint32 BaseIndex = Vertices.Num();

		for (uint8 i = 0; i < 4; i++)
		{
			Vertices.Add(GetCubeVertices(HalfBlockSize)[Face[i]] + (BlockCoords * BlockSize) + HalfBlockSize);
			UVs.Add(FaceUVs[i]);
			VertexColors.Emplace(255, 255, 255, FaceIndex);
			Normals.Add(FaceNormals[FaceIndex]);
		}
    	
		Triangles.Add(BaseIndex);
		Triangles.Add(BaseIndex + 1);
		Triangles.Add(BaseIndex + 2);
        
		Triangles.Add(BaseIndex);
		Triangles.Add(BaseIndex + 2);
		Triangles.Add(BaseIndex + 3);
	}
}

void ARCWorldChunk::CreateProceduralMesh()
{
	ProceduralMesh->ClearAllMeshSections();
	for (uint8 i = 0; i < ChunkMeshes.Num(); i++)
	{
		if (ChunkMeshes[i].Vertices.Num() > 0)
		{
			ProceduralMesh->CreateMeshSection(i, ChunkMeshes[i].Vertices, ChunkMeshes[i].Triangles, ChunkMeshes[i].Normals, ChunkMeshes[i].UVs, ChunkMeshes[i].VertexColors, ChunkMeshes[i].Tangents, true);
		}
	}
}

bool ARCWorldChunk::UpdateChunkBlocksDataAtBlockCoords(const EBlockType BlockTypeToSpawn, const FVector& BlockCoords)
{
	const int32 BlockIndex = GetBlockIndex(BlockCoords.X, BlockCoords.Y, BlockCoords.Z);
	ChunkBlocksData[BlockIndex] = BlockTypeToSpawn;

	RenderChunk();
	
	return true;
}

bool ARCWorldChunk::SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords)
{
	const FVector LocalGridCoords = GetLocalGridCoords(GridCoords);

	UpdateChunkBlocksDataAtBlockCoords(BlockTypeToSpawn, LocalGridCoords);
	CheckIfChunkBorderCubeGotUpdated(BlockTypeToSpawn, LocalGridCoords.X, LocalGridCoords.Y, LocalGridCoords.Z);
	
	return true;
}

void ARCWorldChunk::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MiningTimerHandle);
	}
}

void ARCWorldChunk::StartMining()
{
	bIsMining = true;

	if (LookAtBlockCoords.Z == 0)
		return;

	const EBlockType Type = GetBlockType(LookAtBlockCoords.X, LookAtBlockCoords.Y, LookAtBlockCoords.Z);

	if (Type == EBlockType::Air)
		return;
	
	CurrentMiningBlockData = WorldManager->GetDataAssetBlockFromType(Type);

	if (const UWorld* World = GetWorld())
		World->GetTimerManager().SetTimer(
		MiningTimerHandle,
		this,
		&ARCWorldChunk::UpdateMiningProgress,
		MiningUpdateInterval,
		true
	);
}

void ARCWorldChunk::StopMining()
{
	bIsMining = false;
	CurrentMinedTime = 0;

	if (const UWorld* World = GetWorld())
		World->GetTimerManager().ClearTimer(MiningTimerHandle);
}

void ARCWorldChunk::OnBlockMined()
{
	const EBlockType Type = GetBlockType(LookAtBlockCoords.X, LookAtBlockCoords.Y, LookAtBlockCoords.Z);
	WorldManager->OnBlockMined(Type);
	StopMining();
	UpdateChunkBlocksDataAtBlockCoords(EBlockType::Air, LookAtBlockCoords);
	CheckIfChunkBorderCubeGotUpdated(EBlockType::Air, LookAtBlockCoords.X, LookAtBlockCoords.Y, LookAtBlockCoords.Z);
}

void ARCWorldChunk::UpdateMiningProgress()
{
	CurrentMinedTime += MiningUpdateInterval;

	if (CurrentMinedTime >= CurrentMiningBlockData->GetMineTime())
	{
		OnBlockMined();
	}
}

void ARCWorldChunk::LookAtBlockChanged()
{
	if (bIsMining)
	{
		StopMining();
	}
}


void ARCWorldChunk::CheckIfChunkBorderCubeGotUpdated(const EBlockType UpdatedBlockType, const int8 X, const int8 Y, const int8 Z) const
{
	if (X == 0 || X == ChunkSize - 1)
	{
		const int8 NewCoordX = FMath::Clamp(X - 1, -1, 1);
		
		ARCWorldChunk* NextToChunk = WorldManager->GetChunkAtWorldCoords(ChunkWorldCoords.X + ChunkSize * NewCoordX, ChunkWorldCoords.Y);
		
		const int8 BlockNewCoords = ChunkSize - (X + (NewCoordX + 1));
		NextToChunk->UpdateChunkBlocksDataAtBlockCoords(UpdatedBlockType, FVector(BlockNewCoords, Y, Z));
	}
	if (Y == 0 || Y == ChunkSize - 1)
	{
		const int8 NewCoordY = FMath::Clamp(Y - 1, -1, 1);
		
		ARCWorldChunk* NextToChunk = WorldManager->GetChunkAtWorldCoords(ChunkWorldCoords.X, ChunkWorldCoords.Y + ChunkSize * NewCoordY);

		const int8 BlockNewCoords = ChunkSize - (Y + (NewCoordY + 1));
		NextToChunk->UpdateChunkBlocksDataAtBlockCoords(UpdatedBlockType, FVector(X, BlockNewCoords, Z));
	}
}

void ARCWorldChunk::SetCurrentlyLookAtBlock(const FVector& Coords)
{
	const FVector NewCoords = GetLocalGridCoords(Coords);
	
	if (NewCoords != LookAtBlockCoords)
	{
		LookAtBlockCoords = NewCoords;
		LookAtBlockChanged();
	}
}

/***************************************************/
/*					  GETTERS						/
/***************************************************/
int32 ARCWorldChunk::GetBlockIndex(int8 X, int8 Y, int8 Z) const
{
	return (X + 1) * (ChunkSizeWithBorder * ChunkHeight) + 
		   (Y + 1) * ChunkHeight + 
		   Z;
}

EBlockType ARCWorldChunk::GetBlockType(int8 X, int8 Y, int8 Z) const
{
	const int32 BlockIndex = GetBlockIndex(X, Y, Z);
	return ChunkBlocksData[BlockIndex];
}

TArray<float> ARCWorldChunk::GeneratePerlinNoise() const
{
	return URCPerlinNoise::GenerateHeightMap(
		ChunkSizeWithBorder,
		ChunkSizeWithBorder,
		WorldSettings->PerlinNoiseScale,
		FVector2D(ChunkWorldCoords.X - (1.f/ChunkSize),
		ChunkWorldCoords.Y - (1.f/ChunkSize)),
		WorldSettings->SeedOffset
		);
}

uint8 ARCWorldChunk::GetNoiseHeightAt(int8 X, int8 Z)
{
	if (PerlinNoise.Num() == 0)
		return 0;
	
	const int NoiseIndex = X + Z * (ChunkSizeWithBorder);
	
	if (NoiseIndex >= 0 && NoiseIndex < PerlinNoise.Num())
	{
		float NoiseHeight = PerlinNoise[NoiseIndex];
		const uint8 Height = NoiseHeight * ChunkHeight;
		return Height;
	}
    
	return 0;
}

EBlockType ARCWorldChunk::GetBlockTypeFromHeight(const int8 TerrainHeight, const int8 BlockHeight) const
{	
	if (BlockHeight > TerrainHeight)
		return EBlockType::Air;
	else if (BlockHeight == TerrainHeight && BlockHeight < ChunkHeight - (SnowLevel))
		return EBlockType::Grass;
	else if (BlockHeight >= TerrainHeight - (RockLevel) && BlockHeight < ChunkHeight - (SnowLevel))
		return EBlockType::Dirt;
	else if (BlockHeight <= TerrainHeight - (RockLevel))
		return EBlockType::Stone;
	else if (BlockHeight >= ChunkHeight - (SnowLevel))
		return EBlockType::Snow;
	
	return EBlockType::Air;
}

bool ARCWorldChunk::IsBlockAtCoords(const int8 X, const int8 Y, const int8 Z) const
{
	if (Z == -1)
		return true;
	
	const EBlockType Type = GetBlockType(X, Y, Z);
	
	if (Type == EBlockType::Air)
		return false;
	return true;
}

uint8 ARCWorldChunk::GetBlockFaceBitmaskVisibility(int8 X, int8 Y, int8 Z) const
{
	uint8 Bitmask = 0;
	
	if (!IsBlockAtCoords(X, Y - 1, Z))						Bitmask |= 0x01;
	if (Y + 1 <= ChunkSize && !IsBlockAtCoords(X, Y + 1, Z)) Bitmask |= 0x02;
	if (!IsBlockAtCoords(X - 1, Y, Z))						Bitmask |= 0x04;
	if (X + 1 <= ChunkSize && !IsBlockAtCoords(X + 1, Y, Z)) Bitmask |= 0x08;
	if (!IsBlockAtCoords(X, Y, Z + 1))						Bitmask |= 0x10;
	if (Z - 1 >= 0 && !IsBlockAtCoords(X, Y, Z - 1))				Bitmask |= 0x20;
    
	return Bitmask;
}

FVector ARCWorldChunk::GetLocalGridCoords(const FVector& GridCoords) const
{
	return GridCoords - ChunkWorldCoords;
}


