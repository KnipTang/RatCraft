// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Blocks/RCBlockStatics.h"
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

	void Init(class ARCWorldManager* InWorldManager);

	virtual void OnInteract() override;
	virtual void EndInteract() override;

	void SetRender(const bool Render);
	
	void SetCurrentlyLookAtBlock(const FVector& Coords);
	bool IsMining() const { return bIsMining; }
	
	bool SpawnBlock(const EBlockType BlockTypeToSpawn, const FVector& GridCoords);

protected:
	virtual void BeginPlay() override;

private:
	void InitChunckBlockData();
	
	void RenderChunck();
	void GenerateBlockFaces(const FVector& Coords);

	bool UpdateChunckBlocksDataAtBlockCoords(const EBlockType BlockTypeToSpawn, const FVector& BlockCoords);
	void CheckIfChunkBorderCubeGotUpdated(const EBlockType UpdatedBlockType, const FVector& Coords) const;
	
	//MINING
	void StartMining();
	void StopMining();
	void OnBlockMined();
	void UpdateMiningProgress();
	UPROPERTY()
	class URCDataAssetBlock* CurrentMiningBlockData;

	void LookAtBlockChanged();
	
	//GETTERS
	float GetNoiseHeightAt(int X, int Z);
	TArray<float> GeneratePerlinNoise() const;
	
	struct FBlockFaceVisibility GetBlockFaceVisibilityFromCoords(const FVector& Coords) const;
	class URCDataAssetBlock* GetDataAssetBlockFromType(EBlockType BlockType) const;
	FVector GetLocalGridCoords(const FVector& GridCoords) const;

	bool IsBlockAtCoords(const FVector& Coords) const;
private:
	UPROPERTY()
	const class URCWorldSettings* WorldSettings;

	UPROPERTY()
	class ARCWorldManager* WorldManager;
	
	UPROPERTY(VisibleAnywhere)
	class UProceduralMeshComponent* ProceduralMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Blocks")
	TMap<EBlockType, class URCDataAssetBlock*> BlockDataAsset;
	
	bool bIsRendered;
	
	TMap<FVector /*Coords*/, EBlockType> ChunckBlocksData;
	TArray<FChunckMesh> ChunckMeshes;

	FVector LookAtBlockCoords;
	
	TArray<float> PerlinNoise;

	FVector ChunckWorldCoords;
	FVector2D ChunckGridCoords;
	
	//Mining
	FTimerHandle MiningTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float MiningUpdateInterval = 0.1f;
	
	float CurrentMinedTime;
	bool bIsMining;
};
