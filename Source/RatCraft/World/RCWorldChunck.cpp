// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldChunck.h"
#include "ProceduralMeshComponent.h"
#include "RCWorldManager.h"
#include "RCWorldSettings.h"
#include "Blocks/RCBlockStatics.h"
#include "Blocks/RCDataAssetBlock.h"
#include "PerlinNoise/RCPerlinNoise.h"

#pragma optimize("", off)

// Sets default values
ARCWorldChunck::ARCWorldChunck()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->SetCollisionResponseToAllChannels(ECR_Block);
	RootComponent = ProceduralMesh;
}

void ARCWorldChunck::Init(ARCWorldManager* InWorldManager)
{
	WorldManager = InWorldManager;
}

void ARCWorldChunck::BeginPlay()
{
	Super::BeginPlay();

	WorldSettings = URCWorldSettings::GetSettings();

	ProceduralMesh->SetVisibility(bIsRendered);

	ChunckWorldCoords = GetActorLocation() / WorldSettings->BlockSize;
	ChunckGridCoords = FVector2D( ChunckWorldCoords / WorldSettings->ChunckSize);

	PerlinNoise = GeneratePerlinNoise();
	InitChunckBlockData();
	RenderChunck();
}

void ARCWorldChunck::OnInteract()
{
	StartMining();
}

void ARCWorldChunck::EndInteract()
{
	StopMining();
}

void ARCWorldChunck::SetRender(const bool Render)
{
	if (bIsRendered == Render)
		return;

	bIsRendered = Render;
	
	if (Render)
	{
		ProceduralMesh->SetVisibility(true);
	}
	else
	{
		ProceduralMesh->SetVisibility(false);
	}
}

void ARCWorldChunck::InitChunckBlockData()
{
	for (int X = -1; X < WorldSettings->ChunckSize + 1; X++)
	{
		for (int Z = -1; Z < WorldSettings->ChunckSize + 1; Z++)
		{
			float NoiseHeight = GetNoiseHeightAt(X + 1, Z + 1);
			NoiseHeight = FMath::Clamp(NoiseHeight, 0.4f, 1.f);
			int TerrainHeight = FMath::RoundToInt(NoiseHeight * WorldSettings->ChunckHeight);
			for (int Y = 0; Y < WorldSettings->ChunckHeight; Y++)
			{
				const FVector Coords = FVector(X, Z, Y);
				ChunckBlocksData.Add(Coords, GetBlockTypeFromHeight(WorldSettings, TerrainHeight, Y));
			}
		}
	}
}

void ARCWorldChunck::RenderChunck()
{
	ChunckMeshes = {};
	ChunckMeshes.SetNum(static_cast<uint8>(EBlockType::Air));
	
	for (TPair<FVector /*Coords*/, EBlockType> BlockData : ChunckBlocksData)
	{
		if (BlockData.Value == EBlockType::Air)
			continue;
		if (BlockData.Key.X == -1 || BlockData.Key.Y == -1 || BlockData.Key.X == WorldSettings->ChunckSize || BlockData.Key.Y == WorldSettings->ChunckSize)
			continue;

		GenerateBlockFaces(BlockData.Key);
	}
	
	ProceduralMesh->ClearAllMeshSections();
	for (uint8 i = 0; i < ChunckMeshes.Num(); i++)
	{
		if (ChunckMeshes[i].Vertices.Num() > 0)
		{
			ProceduralMesh->CreateMeshSection(i, ChunckMeshes[i].Vertices, ChunckMeshes[i].Triangles, ChunckMeshes[i].Normals, ChunckMeshes[i].UVs, ChunckMeshes[i].VertexColors, ChunckMeshes[i].Tangents, true);
		}
		
		UMaterialInterface* Material = GetDataAssetBlockFromType(static_cast<EBlockType>(i))->GetMaterial();
		ProceduralMesh->SetMaterial(i, Material);
	}
}

void ARCWorldChunck::GenerateBlockFaces(const FVector& Coords)
{
	EBlockType BlockType = ChunckBlocksData[Coords];
	const uint8 MaterialIndex = static_cast<int32>(BlockType);
	
	TArray<FVector>& Vertices = ChunckMeshes[MaterialIndex].Vertices;
	TArray<int32>& Triangles = ChunckMeshes[MaterialIndex].Triangles;
	TArray<FVector>& Normals = ChunckMeshes[MaterialIndex].Normals;
	TArray<FVector2D>& UVs = ChunckMeshes[MaterialIndex].UVs;
	TArray<FColor>& VertexColors = ChunckMeshes[MaterialIndex].VertexColors;

	const FBlockFaceVisibility BlockFaceVisibility = GetBlockFaceVisibilityFromCoords(Coords);

	for (int32 FaceIndex = 0; FaceIndex < Faces.Num(); FaceIndex++)
	{
		bool FaceVisible = BlockFaceVisibility.Faces[FaceIndex];
		if (!FaceVisible)
			continue;
		
		const TArray<int32>& Face = Faces[FaceIndex];
		int32 BaseIndex = Vertices.Num();

		for (int32 i = 0; i < 4; i++)
		{
			Vertices.Add(GetCubeVertices(WorldSettings->GetHalfBlockSize())[Face[i]] + (Coords * WorldSettings->BlockSize) + WorldSettings->GetHalfBlockSize());
        	
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

bool ARCWorldChunck::UpdateChunckBlocksDataAtBlockCoords(const EBlockType BlockTypeToSpawn, const FVector& BlockCoords)
{
	if (!ChunckBlocksData.Contains(BlockCoords))
		return false;

	ChunckBlocksData.FindChecked(BlockCoords) = BlockTypeToSpawn;

	RenderChunck();
	
	return true;
}

bool ARCWorldChunck::SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords)
{
	const FVector LocalGridCoords = GetLocalGridCoords(GridCoords);

	UpdateChunckBlocksDataAtBlockCoords(BlockTypeToSpawn, LocalGridCoords);
	CheckIfChunkBorderCubeGotUpdated(BlockTypeToSpawn, LocalGridCoords);
	
	return true;
}

void ARCWorldChunck::StartMining()
{
	bIsMining = true;

	if (LookAtBlockCoords.Z == 0)
		return;

	if (!ChunckBlocksData.Contains(LookAtBlockCoords))
		return;
	
	const EBlockType Type = ChunckBlocksData.FindChecked(LookAtBlockCoords);

	if (Type == EBlockType::Air)
		return;
	
	CurrentMiningBlockData = GetDataAssetBlockFromType(Type);

	if (const UWorld* World = GetWorld())
		World->GetTimerManager().SetTimer(
		MiningTimerHandle,
		this,
		&ARCWorldChunck::UpdateMiningProgress,
		MiningUpdateInterval,
		true
	);
}

void ARCWorldChunck::StopMining()
{
	bIsMining = false;
	CurrentMinedTime = 0;

	if (const UWorld* World = GetWorld())
		World->GetTimerManager().ClearTimer(MiningTimerHandle);
}

void ARCWorldChunck::OnBlockMined()
{
	StopMining();
	UpdateChunckBlocksDataAtBlockCoords(EBlockType::Air, LookAtBlockCoords);
	CheckIfChunkBorderCubeGotUpdated(EBlockType::Air, LookAtBlockCoords);
}

void ARCWorldChunck::UpdateMiningProgress()
{
	CurrentMinedTime += MiningUpdateInterval;

	UE_LOG(LogTemp, Warning, TEXT("%f"), CurrentMinedTime);

	if (CurrentMinedTime >= CurrentMiningBlockData->GetMineTime())
	{
		OnBlockMined();
	}
}

void ARCWorldChunck::LookAtBlockChanged()
{
	if (bIsMining)
	{
		StopMining();
	}
}


void ARCWorldChunck::CheckIfChunkBorderCubeGotUpdated(const EBlockType UpdatedBlockType, const FVector& Coords) const
{
	if (Coords.X == 0 || Coords.X == WorldSettings->ChunckSize - 1)
	{
		const int NewCoordX = FMath::Clamp(Coords.X - 1, -1, 1);
		
		ARCWorldChunck* NextToChunk = WorldManager->GetChunkAtWorldCoords(ChunckWorldCoords.X + WorldSettings->ChunckSize * NewCoordX, ChunckGridCoords.Y);
		
		const int BlockNewCoords = WorldSettings->ChunckSize - (Coords.X + (NewCoordX + 1));
		NextToChunk->UpdateChunckBlocksDataAtBlockCoords(UpdatedBlockType, FVector(BlockNewCoords, Coords.Y, Coords.Z));
	}
	if (Coords.Y == 0 || Coords.Y == WorldSettings->ChunckSize - 1)
	{
		const int NewCoordY = FMath::Clamp(Coords.Y - 1, -1, 1);
		
		ARCWorldChunck* NextToChunk = WorldManager->GetChunkAtWorldCoords(ChunckWorldCoords.X, ChunckGridCoords.Y + WorldSettings->ChunckSize * NewCoordY);

		const int BlockNewCoords = WorldSettings->ChunckSize - (Coords.Y + (NewCoordY + 1));
		NextToChunk->UpdateChunckBlocksDataAtBlockCoords(UpdatedBlockType, FVector(Coords.X, BlockNewCoords, Coords.Z));
	}
}

void ARCWorldChunck::SetCurrentlyLookAtBlock(const FVector& Coords)
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
TArray<float> ARCWorldChunck::GeneratePerlinNoise() const
{
	return URCPerlinNoise::GenerateHeightMap(WorldSettings->ChunckSize + 2, WorldSettings->ChunckSize + 2, WorldSettings->PerlinNoiseScale, FVector2D(ChunckWorldCoords.X - (1.f/WorldSettings->ChunckSize), ChunckWorldCoords.Y - (1.f/WorldSettings->ChunckSize)), WorldSettings->Seed);
}

float ARCWorldChunck::GetNoiseHeightAt(int X, int Z)
{
	if (PerlinNoise.Num() == 0)
		return 0.0f;
	
	const int NoiseIndex = X + Z * (WorldSettings->ChunckSize + 2);
	
	if (NoiseIndex >= 0 && NoiseIndex < PerlinNoise.Num())
	{
		return PerlinNoise[NoiseIndex];
	}
    
	return 0.0f;
}

bool ARCWorldChunck::IsBlockAtCoords(const FVector& Coords) const
{
	if (Coords.Z == -1)
		return true;
	
	if (!ChunckBlocksData.Contains(Coords) || ChunckBlocksData[Coords] == EBlockType::Air)
		return false;
	return true;
}

FBlockFaceVisibility ARCWorldChunck::GetBlockFaceVisibilityFromCoords(const FVector& Coords) const
{
	const bool South	= ( !IsBlockAtCoords(FVector(Coords.X, Coords.Y - 1, Coords.Z)));
	const bool North	= ( !IsBlockAtCoords(FVector(Coords.X, Coords.Y + 1, Coords.Z)) && (Coords.Y + 1 <= WorldSettings->ChunckSize) );
	const bool West		= ( !IsBlockAtCoords(FVector(Coords.X - 1, Coords.Y, Coords.Z)));
	const bool East		= ( !IsBlockAtCoords(FVector(Coords.X + 1, Coords.Y, Coords.Z)) && (Coords.X + 1 <= WorldSettings->ChunckSize) );
	const bool Top		= ( !IsBlockAtCoords(FVector(Coords.X, Coords.Y, Coords.Z + 1)) );
	const bool Bottom	= ( !IsBlockAtCoords(FVector(Coords.X, Coords.Y, Coords.Z - 1)) && (Coords.Z - 1 >= 0) );
		
	const FBlockFaceVisibility BlockFaceVisibility{South, North, West, East, Top, Bottom};

	return BlockFaceVisibility;
}

class URCDataAssetBlock* ARCWorldChunck::GetDataAssetBlockFromType(const EBlockType BlockType) const
{
	if (BlockDataAsset.Contains(BlockType))
		return BlockDataAsset.FindChecked(BlockType);
	return nullptr;
}

FVector ARCWorldChunck::GetLocalGridCoords(const FVector& GridCoords) const
{
	return GridCoords - ChunckWorldCoords;
}


