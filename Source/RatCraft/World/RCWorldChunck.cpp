// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldChunck.h"
#include "ProceduralMeshComponent.h"
#include "RCWorldSettings.h"
#include "Blocks/RCBlock.h"
#include "Blocks/RCDataAssetBlock.h"
#include "PerlinNoise/RCPerlinNoise.h"

#pragma optimize("", off)

// Sets default values
ARCWorldChunck::ARCWorldChunck()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->SetCollisionResponseToAllChannels(ECR_Block);
	RootComponent = ProceduralMesh;

	WorldSettings = URCWorldSettings::GetSettings();

	Faces = {
		{3, 2, 1, 0}, // South
		{6, 7, 4, 5}, // North
		{7, 3, 0, 4}, // West
		{2, 6, 5, 1}, // East
		{7, 6, 2, 3}, // Top
		{0, 1, 5, 4}  // Bottom
	};
}

void ARCWorldChunck::BeginPlay()
{
	Super::BeginPlay();

	ChunckGridCoords = GetActorLocation() / WorldSettings->BlockSize;

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

void ARCWorldChunck::InitChunckBlockData()
{
	for (int X = 0; X < WorldSettings->ChunckSize; X++)
	{
		for (int Z = 0; Z < WorldSettings->ChunckSize; Z++)
		{
			const float NoiseHeight = GetNoiseHeightAt(X, Z);
			int TerrainHeight = FMath::RoundToInt(NoiseHeight * WorldSettings->ChunckHeight);
			TerrainHeight = FMath::Clamp(TerrainHeight, 0, WorldSettings->ChunckHeight - 1);
			
			for (int Y = 0; Y < WorldSettings->ChunckHeight; Y++)
			{
				const FVector Coords = FVector(X, Z, Y);
				ChunckBlocksData.Add(Coords, GetBlockTypeFromHeight(TerrainHeight, Y));
			}
		}
	}
	
}

void ARCWorldChunck::RenderChunck()
{
	ChunckMeshes = {};
	
	for (TPair<FVector /*Coords*/, EBlockType> BlockData : ChunckBlocksData)
	{
		if (BlockData.Value == EBlockType::Air)
			continue;

		GenerateBlockFaces(BlockData.Key);
	}

	ProceduralMesh->ClearAllMeshSections();
    ProceduralMesh->CreateMeshSection(0, ChunckMeshes.Vertices, ChunckMeshes.Triangles, ChunckMeshes.Normals, ChunckMeshes.UVs, ChunckMeshes.VertexColors, ChunckMeshes.Tangents, true);
    if (UMaterialInterface* Material = BlockDataAsset.FindChecked(EBlockType::Grass)->GetMaterial())
    {
    	ProceduralMesh->SetMaterial(0, Material);
    }
}

void ARCWorldChunck::GenerateBlockFaces(const FVector& Coords)
{
	TArray<FVector>& Vertices = ChunckMeshes.Vertices;
	TArray<int32>& Triangles = ChunckMeshes.Triangles;
	TArray<FVector>& Normals = ChunckMeshes.Normals;
	TArray<FVector2D>& UVs = ChunckMeshes.UVs;
	TArray<FColor>& VertexColors = ChunckMeshes.VertexColors;

	const FBlockFaceVisibility BlockFaceVisibility = GetBlockFaceVisibilityFromCoords(Coords);

	TArray<TArray<int32>> FinalFaces {};

	for (int32 FaceIndex = 0; FaceIndex < Faces.Num(); FaceIndex++)
	{
		bool FaceVisible = BlockFaceVisibility.Faces[FaceIndex];
		if (!FaceVisible)
			continue;

		FinalFaces.Add(Faces[FaceIndex]);
	}
	
    for (int32 FaceIndex = 0; FaceIndex < FinalFaces.Num(); FaceIndex++)
    {
        const TArray<int32>& Face = FinalFaces[FaceIndex];
        int32 BaseIndex = Vertices.Num();

        for (int32 i = 0; i < 4; i++)
        {
            Vertices.Add(WorldSettings->CubeVertices[Face[i]] + (Coords * WorldSettings->BlockSize) + WorldSettings->HalfBlockSize);
        	
            if (i == 0) UVs.Add(FVector2D(0, 1));
            else if (i == 1) UVs.Add(FVector2D(1, 1));
            else if (i == 2) UVs.Add(FVector2D(1, 0));
            else UVs.Add(FVector2D(0, 0));
        	
            VertexColors.Add(GetBlockColorFromBlockType(ChunckBlocksData[Coords]));
        }
    	
        Triangles.Add(BaseIndex);
        Triangles.Add(BaseIndex + 1);
        Triangles.Add(BaseIndex + 2);
        
        Triangles.Add(BaseIndex);
        Triangles.Add(BaseIndex + 2);
        Triangles.Add(BaseIndex + 3);
    	
        for (int32 i = 0; i < 4; i++)
        {
            Normals.Add(WorldSettings->FaceNormals[FaceIndex]);
        }
    }
}

void ARCWorldChunck::UpdateChunckMesh()
{
	RenderChunck();
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

bool ARCWorldChunck::SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords)
{
	const FVector LocalGridCoords = GetLocalGridCoords(GridCoords);

	if (!ChunckBlocksData.Contains(LocalGridCoords))
		return false;

	ChunckBlocksData.FindChecked(LocalGridCoords) = BlockTypeToSpawn;
	
	UpdateChunckMesh();
	
	return true;
}

bool ARCWorldChunck::CanSpawnBlockAtGridCoords(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float PlayerColliderSize) const
{
	return (
		!IsPlayerObstructing(NewBlockGridCoords, PlayerGridCoords, PlayerColliderSize)
		&& NewBlockGridCoords.Z - ChunckGridCoords.Z <= WorldSettings->ChunckHeight
		);
}

bool ARCWorldChunck::IsPlayerObstructing(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float PlayerColliderSize) const
{
	const float HalfBlockSizeCoordsSize = WorldSettings->HalfBlockSize / WorldSettings->BlockSize;
	const float Distance = 
		FMath::Abs((NewBlockGridCoords.X + HalfBlockSizeCoordsSize) - PlayerGridCoords.X) +
		FMath::Abs((NewBlockGridCoords.Y + HalfBlockSizeCoordsSize) - PlayerGridCoords.Y) +
		FMath::Abs((NewBlockGridCoords.Z + HalfBlockSizeCoordsSize) - PlayerGridCoords.Z);
	
	if (Distance >= 1 + PlayerColliderSize / WorldSettings->BlockSize)
	{
		return false;
	}
    
	return true;
}

void ARCWorldChunck::StartMining()
{
	bIsMining = true;

	if (!ChunckBlocksData.Contains(LookAtBlockCoords))
		return;
	
	const EBlockType Type = ChunckBlocksData.FindChecked(LookAtBlockCoords);
	CurrentMiningBlockData = BlockDataAsset.FindChecked(Type);

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
	ChunckBlocksData[LookAtBlockCoords] = EBlockType::Air;
	StopMining();
	UpdateChunckMesh();
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

/***************************************************/
/*					  GETTERS						/
/***************************************************/
TArray<float> ARCWorldChunck::GeneratePerlinNoise() const
{
	return URCPerlinNoise::GenerateHeightMap(WorldSettings->ChunckSize, WorldSettings->ChunckSize, WorldSettings->PerlinNoiseScale, FVector2D(ChunckGridCoords.X, ChunckGridCoords.Y));
}

float ARCWorldChunck::GetNoiseHeightAt(int X, int Z)
{
	URCWorldSettings::GetSettings()->BlockSize;
	if (PerlinNoise.Num() == 0)
		return 0.0f;
	
	const int NoiseIndex = X + Z * WorldSettings->ChunckSize;
    
	if (NoiseIndex >= 0 && NoiseIndex < PerlinNoise.Num())
	{
		return PerlinNoise[NoiseIndex];
	}
    
	return 0.0f;
}

EBlockType ARCWorldChunck::GetBlockTypeFromHeight(const int TerrainHeight, const int BlockHeight) const
{
	if (BlockHeight > TerrainHeight)
		return EBlockType::Air;
	else if (BlockHeight >= TerrainHeight - WorldSettings->RockLevel && BlockHeight < WorldSettings->SnowLevel)
		return EBlockType::Grass;
	else if (BlockHeight < TerrainHeight - WorldSettings->RockLevel)
		return EBlockType::Stone;
	else if (BlockHeight >= WorldSettings->SnowLevel)
		return EBlockType::Snow;
	
	return EBlockType::Air;
}

bool ARCWorldChunck::IsBlockAtCoords(const FVector& Coords) const
{
	if (!ChunckBlocksData.Contains(Coords) || ChunckBlocksData[Coords] == EBlockType::Air)
		return false;
	return true;
}

FBlockFaceVisibility ARCWorldChunck::GetBlockFaceVisibilityFromCoords(const FVector& Coords) const
{
	const bool Top		= !IsBlockAtCoords(FVector(Coords.X, Coords.Y, Coords.Z + 1));
	const bool Bottom	= !IsBlockAtCoords(FVector(Coords.X, Coords.Y, Coords.Z - 1));
	const bool North	= !IsBlockAtCoords(FVector(Coords.X, Coords.Y + 1, Coords.Z));
	const bool South	= !IsBlockAtCoords(FVector(Coords.X, Coords.Y - 1, Coords.Z));
	const bool East		= !IsBlockAtCoords(FVector(Coords.X + 1, Coords.Y, Coords.Z));
	const bool West		= !IsBlockAtCoords(FVector(Coords.X - 1, Coords.Y, Coords.Z));
		
	const FBlockFaceVisibility BlockFaceVisibility{Top, Bottom, North, East, South, West};

	return BlockFaceVisibility;
}

FColor ARCWorldChunck::GetBlockColorFromBlockType(const EBlockType BlockTypeToSpawn)
{
	switch (BlockTypeToSpawn)
	{
	case EBlockType::Air:
		break;
	case EBlockType::Grass:
		return FColor::Green;
		break;
	case EBlockType::Stone:
		return FColor::Black;
		break;
	case EBlockType::Snow:
		return FColor::White;
		break;
	}
	
	return FColor::Purple;
}

FVector ARCWorldChunck::GetLocalGridCoords(const FVector& GridCoords) const
{
	return GridCoords - ChunckGridCoords;
}


