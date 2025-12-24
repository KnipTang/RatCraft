// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/RCBlockTypes.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "RCWorldChunck.generated.h"

USTRUCT()
struct FChunckMesh
{
GENERATED_BODY()
	
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;
};

UCLASS()
class RATCRAFT_API ARCWorldChunck : public AActor
{
	GENERATED_BODY()
	
public:	
	ARCWorldChunck();

protected:
	virtual void BeginPlay() override;

private:
	void InitChunckBlockData();
	
	void RenderChunck();
	void RenderBlock(TPair<FVector /*Coords*/, EBlockType> BlockData, const struct FBlockFaceVisibility BlockfaceVisibility);
	void GenerateFace(TPair<FVector, EBlockType> BlockData, const struct FBlockFaceVisibility BlockfaceVisibility);
	
	//GETTERS
	float GetNoiseHeightAt(int X, int Z);
	TArray<float> GeneratePerlinNoise() const;
	
	EBlockType GetBlockTypeFromHeight(const int TerrainHeight, const int BlockHeight) const;
	struct FBlockFaceVisibility GetBlockFaceVisibilityFromCoords(const FVector& Coords) const;
	bool IsBlockAtCoords(const FVector& Coords) const;
	FColor GetBlockColorFromBlockType(const EBlockType BlockTypeToSpawn);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int ChunckSize = 16;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int ChunckHeight = 50;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float GridScale = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int SnowLevel = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int RockLevel = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int BlockSize = 100.f;
	int HalfBlockSize;
	
	TMap<FVector /*Coords*/, EBlockType> ChunckBlocksData;
	FChunckMesh ChunckMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "Blocks")
	TMap<EBlockType, class URCDataAssetBlock*> BlockDataAsset;

	UPROPERTY(VisibleAnywhere)
	class UProceduralMeshComponent* ProceduralMesh;
	
	TArray<FVector> FaceNormals;
	TArray<FVector> CubeVertices;
	
	TArray<float> PerlinNoise;
};
