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

	SetRender(false);
	
	WorldSettings = URCWorldSettings::GetSettings();

	ChunkWorldCoords = GetActorLocation() / WorldSettings->BlockSize;
	ChunkGridCoords = FVector2D( ChunkWorldCoords / WorldSettings->ChunkSize);

	for (uint8 i = 0; i < static_cast<uint8>(EBlockType::Air); i++)
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

void ARCWorldChunk::OnInteract()
{
	StartMining();
}

void ARCWorldChunk::EndInteract()
{
	StopMining();
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
	for (int8 X = -1; X < WorldSettings->ChunkSize + 1; X++)
	{
		for (int8 Z = -1; Z < WorldSettings->ChunkSize + 1; Z++)
		{
			const uint8 TerrainHeight = GetNoiseHeightAt(X + 1, Z + 1);
			for (uint8 Y = 0; Y < WorldSettings->ChunkHeight; Y++)
			{
				const FVector Coords = FVector(X, Z, Y);
				EBlockType BlockType = GetBlockTypeFromHeight(*WorldSettings, TerrainHeight, Y);
				ChunkBlocksData.Add(Coords, BlockType);
			}
		}
	}
}

void ARCWorldChunk::RenderChunk()
{
	ChunkMeshes = {};
	ChunkMeshes.SetNum(static_cast<uint8>(EBlockType::Air));
	
	for (TPair<FVector /*Coords*/, EBlockType>& BlockData : ChunkBlocksData)
	{
		if (BlockData.Value == EBlockType::Air)
			continue;
		if (BlockData.Key.X == -1 || BlockData.Key.Y == -1 || BlockData.Key.X == WorldSettings->ChunkSize || BlockData.Key.Y == WorldSettings->ChunkSize)
			continue;

		GenerateBlockFaces(BlockData.Key);
	}
	
	CreateProceduralMesh();
}

void ARCWorldChunk::GenerateBlockFaces(const FVector& BlockCoords)
{
	EBlockType BlockType = ChunkBlocksData[BlockCoords];
	
	const uint8 MaterialIndex = static_cast<uint8>(BlockType);
	
	TArray<FVector>& Vertices = ChunkMeshes[MaterialIndex].Vertices;
	TArray<int32>& Triangles = ChunkMeshes[MaterialIndex].Triangles;
	TArray<FVector>& Normals = ChunkMeshes[MaterialIndex].Normals;
	TArray<FVector2D>& UVs = ChunkMeshes[MaterialIndex].UVs;
	TArray<FColor>& VertexColors = ChunkMeshes[MaterialIndex].VertexColors;

	const TArray<bool> BlockFaceVisibility = GetBlockFaceVisibilityFromCoords(BlockCoords);

	for (uint8 FaceIndex = 0; FaceIndex < Faces.Num(); FaceIndex++)
	{
		bool FaceVisible = BlockFaceVisibility[FaceIndex];
		if (!FaceVisible)
			continue;
		
		const TArray<uint8>& Face = Faces[FaceIndex];
		uint32 BaseIndex = Vertices.Num();

		for (uint8 i = 0; i < 4; i++)
		{
			Vertices.Add(GetCubeVertices(WorldSettings->GetHalfBlockSize())[Face[i]] + (BlockCoords * WorldSettings->BlockSize) + WorldSettings->GetHalfBlockSize());
        	
			if (i == 0) UVs.Add(FVector2D(0, 1));
			else if (i == 1) UVs.Add(FVector2D(1, 1));
			else if (i == 2) UVs.Add(FVector2D(1, 0));
			else UVs.Add(FVector2D(0, 0));
        	
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
	if (!ChunkBlocksData.Contains(BlockCoords))
		return false;
	
	ChunkBlocksData.FindChecked(BlockCoords) = BlockTypeToSpawn;

	RenderChunk();
	
	return true;
}

bool ARCWorldChunk::SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords)
{
	const FVector LocalGridCoords = GetLocalGridCoords(GridCoords);

	UpdateChunkBlocksDataAtBlockCoords(BlockTypeToSpawn, LocalGridCoords);
	CheckIfChunkBorderCubeGotUpdated(BlockTypeToSpawn, LocalGridCoords);
	
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

	if (!ChunkBlocksData.Contains(LookAtBlockCoords))
		return;
	
	const EBlockType Type = ChunkBlocksData.FindChecked(LookAtBlockCoords);

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
	StopMining();
	UpdateChunkBlocksDataAtBlockCoords(EBlockType::Air, LookAtBlockCoords);
	CheckIfChunkBorderCubeGotUpdated(EBlockType::Air, LookAtBlockCoords);
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


void ARCWorldChunk::CheckIfChunkBorderCubeGotUpdated(const EBlockType UpdatedBlockType, const FVector& Coords) const
{
	if (Coords.X == 0 || Coords.X == WorldSettings->ChunkSize - 1)
	{
		const int NewCoordX = FMath::Clamp(Coords.X - 1, -1, 1);
		
		ARCWorldChunk* NextToChunk = WorldManager->GetChunkAtWorldCoords(ChunkWorldCoords.X + WorldSettings->ChunkSize * NewCoordX, ChunkWorldCoords.Y);
		
		const int BlockNewCoords = WorldSettings->ChunkSize - (Coords.X + (NewCoordX + 1));
		NextToChunk->UpdateChunkBlocksDataAtBlockCoords(UpdatedBlockType, FVector(BlockNewCoords, Coords.Y, Coords.Z));
	}
	if (Coords.Y == 0 || Coords.Y == WorldSettings->ChunkSize - 1)
	{
		const int NewCoordY = FMath::Clamp(Coords.Y - 1, -1, 1);
		
		ARCWorldChunk* NextToChunk = WorldManager->GetChunkAtWorldCoords(ChunkWorldCoords.X, ChunkWorldCoords.Y + WorldSettings->ChunkSize * NewCoordY);

		const int BlockNewCoords = WorldSettings->ChunkSize - (Coords.Y + (NewCoordY + 1));
		NextToChunk->UpdateChunkBlocksDataAtBlockCoords(UpdatedBlockType, FVector(Coords.X, BlockNewCoords, Coords.Z));
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
TArray<float> ARCWorldChunk::GeneratePerlinNoise() const
{
	return URCPerlinNoise::GenerateHeightMap(
		WorldSettings->ChunkSize + 2,
		WorldSettings->ChunkSize + 2,
		WorldSettings->PerlinNoiseScale,
		FVector2D(ChunkWorldCoords.X - (1.f/WorldSettings->ChunkSize),
		ChunkWorldCoords.Y - (1.f/WorldSettings->ChunkSize)),
		WorldSettings->SeedOffset
		);
}

uint8 ARCWorldChunk::GetNoiseHeightAt(int X, int Z)
{
	if (PerlinNoise.Num() == 0)
		return 0;
	
	const int NoiseIndex = X + Z * (WorldSettings->ChunkSize + 2);
	
	if (NoiseIndex >= 0 && NoiseIndex < PerlinNoise.Num())
	{
		float NoiseHeight = PerlinNoise[NoiseIndex];
		const uint8 Height = NoiseHeight * WorldSettings->ChunkHeight;
		return Height;
	}
    
	return 0;
}

bool ARCWorldChunk::IsBlockAtCoords(const FVector& Coords) const
{
	if (Coords.Z == -1)
		return true;
	
	if (!ChunkBlocksData.Contains(Coords) || ChunkBlocksData[Coords] == EBlockType::Air)
		return false;
	return true;
}

TArray<bool> ARCWorldChunk::GetBlockFaceVisibilityFromCoords(const FVector& Coords) const
{
	const bool South	= ( !IsBlockAtCoords(FVector(Coords.X, Coords.Y - 1, Coords.Z)));
	const bool North	= ( !IsBlockAtCoords(FVector(Coords.X, Coords.Y + 1, Coords.Z)) && (Coords.Y + 1 <= WorldSettings->ChunkSize) );
	const bool West		= ( !IsBlockAtCoords(FVector(Coords.X - 1, Coords.Y, Coords.Z)));
	const bool East		= ( !IsBlockAtCoords(FVector(Coords.X + 1, Coords.Y, Coords.Z)) && (Coords.X + 1 <= WorldSettings->ChunkSize) );
	const bool Top		= ( !IsBlockAtCoords(FVector(Coords.X, Coords.Y, Coords.Z + 1)) );
	const bool Bottom	= ( !IsBlockAtCoords(FVector(Coords.X, Coords.Y, Coords.Z - 1)) && (Coords.Z - 1 >= 0) );
		
	const TArray<bool> BlockFaceVisibility{South, North, West, East, Top, Bottom};

	return BlockFaceVisibility;
}

FVector ARCWorldChunk::GetLocalGridCoords(const FVector& GridCoords) const
{
	return GridCoords - ChunkWorldCoords;
}


