// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Blocks/RCBlockStatics.h"
#include "RatCraft/Interactables/RCInteractable.h"
#include "RCWorldChunk.generated.h"

USTRUCT()
struct FChunkMesh
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
class RATCRAFT_API ARCWorldChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	ARCWorldChunk();

	void Init(class ARCWorldManager* InWorldManager);

	void StartMining();
	void StopMining();

	void SetRender(const bool bRender);
	void SetCollision(const bool bCollision);
	
	void SetCurrentlyLookAtBlock(const FVector& Coords);
	bool IsMining() const { return bIsMining; }
	
	bool SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	void InitChunkBlockData();

	void RenderChunk();
	void GenerateBlockFaces(const EBlockType& BlockType, const FVector& BlockCoords);
	
	void CreateProceduralMesh();
	
	bool UpdateChunkBlocksDataAtBlockCoords(const EBlockType BlockTypeToSpawn, const FVector& BlockCoords);
	void CheckIfChunkBorderCubeGotUpdated(const EBlockType UpdatedBlockType, const FVector& Coords) const;
	
	//MINING
	void OnBlockMined();
	void UpdateMiningProgress();
	UPROPERTY()
	class URCDataAssetBlock* CurrentMiningBlockData;

	void LookAtBlockChanged();
	
	//GETTERS
	int32 GetBlockIndex(int8 X, int8 Y, int8 Z) const;
	EBlockType GetBlockType(int8 X, int8 Y, int8 Z) const;
	
	uint8 GetNoiseHeightAt(int X, int Z);
	TArray<float> GeneratePerlinNoise() const;
	
	TArray<bool> GetBlockFaceVisibilityFromCoords(const FVector& Coords) const;
	FVector GetLocalGridCoords(const FVector& GridCoords) const;

	bool IsBlockAtCoords(const FVector& Coords) const;

	EBlockType GetBlockTypeFromHeight(const int TerrainHeight, const int BlockHeight) const;

private:
	UPROPERTY()
	const class URCWorldSettings* WorldSettings;

	UPROPERTY()
	class ARCWorldManager* WorldManager;
	
	UPROPERTY(VisibleAnywhere)
	class UProceduralMeshComponent* ProceduralMesh;
	
	bool bIsRendered = true;
	bool bIsCollision = true;
	
	TArray<EBlockType> ChunkBlocksData;
	TArray<FChunkMesh> ChunkMeshes;

	FVector LookAtBlockCoords;
	
	TArray<float> PerlinNoise;

	FVector ChunkWorldCoords;
	FVector2D ChunkGridCoords;
	
	//Mining
	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float MiningUpdateInterval = 0.1f;
	FTimerHandle MiningTimerHandle;
	
	float CurrentMinedTime;
	bool bIsMining;

	//Cached values
	uint8 ChunkSize;
	uint8 ChunkSizeWithBorder;
	uint8 ChunkHeight;
	int32 TotalBlocks;
	uint8 RockLevel;
	uint8 SnowLevel;
};
