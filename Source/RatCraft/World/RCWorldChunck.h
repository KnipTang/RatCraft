// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/RCBlockTypes.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "RatCraft/Interactables/RCInteractable.h"
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
class RATCRAFT_API ARCWorldChunck : public AActor, public IRCInteractable
{
	GENERATED_BODY()
	
public:	
	ARCWorldChunck();

	virtual void OnInteract() override;
	virtual void EndInteract() override;

	FVector GetGridCoordsFromWorldPosition(const FVector& WorldPosition) const;
	
	void SetCurrentlyLookAtBlock(const FVector& Coords);
	bool IsMining() const { return bIsMining; }
	
	bool SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords, const struct FBlockFaceVisibility BlockFaceVisibility);
	bool CanSpawnBlockAtGridCoords(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float PlayerColliderSize) const;

protected:
	virtual void BeginPlay() override;

private:
	void InitChunckBlockData();
	
	void RenderChunck();
	void RenderBlock(const FVector& Coords);
	void GenerateBlockFaces(const FVector& Coords);

	void UpdateChunckMesh();

	//PLACEMENT
	bool IsPlayerObstructing(const FVector& NewBlockGridCoords, const FVector& PlayerGridCoords, const float PlayerColliderSize) const;

	//MINING
	void StartMining();
	void StopMining();
	void OnBlockMined();
	void UpdateMiningProgress();

	void LookAtBlockChanged();
	
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
	float BlockSize = 100.f;
	float HalfBlockSize;
	
	TMap<FVector /*Coords*/, EBlockType> ChunckBlocksData;
	FChunckMesh ChunckMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "Blocks")
	TMap<EBlockType, class URCDataAssetBlock*> BlockDataAsset;

	UPROPERTY(VisibleAnywhere)
	class UProceduralMeshComponent* ProceduralMesh;
	
	TArray<FVector> FaceNormals;
	TArray<FVector> CubeVertices;

	FVector LookAtBlockCoords;
	
	TArray<float> PerlinNoise;

	//Mining
	FTimerHandle MiningTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float MiningUpdateInterval = 0.1f;
	
	float CurrentMinedTime;
	bool bIsMining;
};
