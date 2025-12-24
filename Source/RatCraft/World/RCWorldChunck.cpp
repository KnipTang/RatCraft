// Fill out your copyright notice in the Description page of Project Settings.


#include "RCWorldChunck.h"

#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "Blocks/RCBlock.h"
#include "Blocks/RCDataAssetBlock.h"
#include "PerlinNoise/RCPerlinNoise.h"

// Sets default values
ARCWorldChunck::ARCWorldChunck()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->SetCollisionResponseToAllChannels(ECR_Block);
	RootComponent = ProceduralMesh;

	HalfBlockSize = BlockSize / 2.f;
	
	FaceNormals = {
		FVector(0, -1, 0),  // South
		FVector(0, 1, 0),   // North
		FVector(-1, 0, 0),  // West
		FVector(1, 0, 0),   // East
		FVector(0, 0, 1),   // Top
		FVector(0, 0, -1)   // Bottom
	};

	CubeVertices = {
		FVector(-HalfBlockSize, -HalfBlockSize, -HalfBlockSize), // Front-bottom-left
		FVector(HalfBlockSize, -HalfBlockSize, -HalfBlockSize),  // Front-bottom-right
		FVector(HalfBlockSize, -HalfBlockSize, HalfBlockSize),   // Front-top-right
		FVector(-HalfBlockSize, -HalfBlockSize, HalfBlockSize),  // Front-top-left
		FVector(-HalfBlockSize, HalfBlockSize, -HalfBlockSize),  // Back-bottom-left
		FVector(HalfBlockSize, HalfBlockSize, -HalfBlockSize),   // Back-bottom-right
		FVector(HalfBlockSize, HalfBlockSize, HalfBlockSize),    // Back-top-right
		FVector(-HalfBlockSize, HalfBlockSize, HalfBlockSize)    // Back-top-left
	};
}

void ARCWorldChunck::BeginPlay()
{
	Super::BeginPlay();

	PerlinNoise = GeneratePerlinNoise();
	InitChunckBlockData();
	RenderChunck();
}

void ARCWorldChunck::InitChunckBlockData()
{
	for (int X = 0; X < ChunckSize; X++)
	{
		for (int Z = 0; Z < ChunckSize; Z++)
		{
			float NoiseHeight = GetNoiseHeightAt(X, Z);
			int TerrainHeight = FMath::RoundToInt(NoiseHeight * ChunckHeight);
			TerrainHeight = FMath::Clamp(TerrainHeight, 1, ChunckHeight - 1);
			
			for (int Y = 0; Y < ChunckHeight; Y++)
			{
				const FVector Coords = FVector(X, Z, Y);
				ChunckBlocksData.Add(Coords, GetBlockTypeFromHeight(TerrainHeight, Y));

			}
		}
	}
	
}

void ARCWorldChunck::RenderChunck()
{
	for (TPair<FVector /*Coords*/, EBlockType> BlockData : ChunckBlocksData)
	{
		if (BlockData.Value == EBlockType::Air)
			continue;
		
		const FBlockFaceVisibility BlockFaceVisibility = GetBlockFaceVisibilityFromCoords(BlockData.Key);
		RenderBlock(BlockData, BlockFaceVisibility);
	}
	
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(ChunckMeshes.Vertices, ChunckMeshes.Triangles, ChunckMeshes.UVs, ChunckMeshes.Normals, ChunckMeshes.Tangents);
    ProceduralMesh->CreateMeshSection(0, ChunckMeshes.Vertices, ChunckMeshes.Triangles, ChunckMeshes.Normals, ChunckMeshes.UVs, ChunckMeshes.VertexColors, ChunckMeshes.Tangents, true);
    	
    if (UMaterialInterface* Material = BlockDataAsset.FindChecked(EBlockType::Grass)->GetMaterial())
    {
    	ProceduralMesh->SetMaterial(0, Material);
    }
}

void ARCWorldChunck::RenderBlock(TPair<FVector, EBlockType> BlockData, const FBlockFaceVisibility BlockfaceVisibility)
{
	GenerateFace(BlockData, BlockfaceVisibility);
}

void ARCWorldChunck::GenerateFace(TPair<FVector, EBlockType> BlockData, const FBlockFaceVisibility BlockfaceVisibility)
{
	TArray<FVector>& Vertices = ChunckMeshes.Vertices;
	TArray<int32>& Triangles = ChunckMeshes.Triangles;
	TArray<FVector>& Normals = ChunckMeshes.Normals;
	TArray<FVector2D>& UVs = ChunckMeshes.UVs;
	TArray<FColor>& VertexColors = ChunckMeshes.VertexColors;
	
    TArray<TArray<int32>> Faces = {
        {3, 2, 1, 0}, // South
		{6, 7, 4, 5}, // North
        {7, 3, 0, 4}, // West
        {2, 6, 5, 1}, // East
        {7, 6, 2, 3}, // Top
        {0, 1, 5, 4}  // Bottom
    };

	TArray<TArray<int32>> FinalFaces {};

	for (int32 FaceIndex = 0; FaceIndex < Faces.Num(); FaceIndex++)
	{
		bool FaceVisible = BlockfaceVisibility.Faces[FaceIndex];
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
            Vertices.Add(CubeVertices[Face[i]] + (BlockData.Key * BlockSize));
        	
            if (i == 0) UVs.Add(FVector2D(0, 1));
            else if (i == 1) UVs.Add(FVector2D(1, 1));
            else if (i == 2) UVs.Add(FVector2D(1, 0));
            else UVs.Add(FVector2D(0, 0));
        	
            VertexColors.Add(GetBlockColorFromBlockType(BlockData.Value));
        }
    	
        Triangles.Add(BaseIndex);
        Triangles.Add(BaseIndex + 1);
        Triangles.Add(BaseIndex + 2);
        
        Triangles.Add(BaseIndex);
        Triangles.Add(BaseIndex + 2);
        Triangles.Add(BaseIndex + 3);
    	
        for (int32 i = 0; i < 4; i++)
        {
            Normals.Add(FaceNormals[FaceIndex]);
        }
    }
}

/***************************************************/
/*					  GETTERS						/
/***************************************************/
float ARCWorldChunck::GetNoiseHeightAt(int X, int Z)
{
	if (PerlinNoise.Num() == 0)
		return 0.0f;
	
	int NoiseIndex = X + Z * ChunckSize;
    
	if (NoiseIndex >= 0 && NoiseIndex < PerlinNoise.Num())
	{
		return PerlinNoise[NoiseIndex];
	}
    
	return 0.0f;
}

TArray<float> ARCWorldChunck::GeneratePerlinNoise() const
{
	return URCPerlinNoise::GenerateHeightMap(ChunckSize, ChunckSize, GridScale, FVector2D());
}

EBlockType ARCWorldChunck::GetBlockTypeFromHeight(const int TerrainHeight, const int BlockHeight) const
{
	if (BlockHeight > TerrainHeight)
		return EBlockType::Air;
	else if (BlockHeight >= TerrainHeight - RockLevel && BlockHeight < SnowLevel)
		return EBlockType::Grass;
	else if (BlockHeight < TerrainHeight - RockLevel)
		return EBlockType::Stone;
	else if (BlockHeight >= SnowLevel)
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


