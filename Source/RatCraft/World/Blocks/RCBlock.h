// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RCBlockTypes.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "RatCraft/Interactables/RCInteractable.h"
#include "RCBlock.generated.h"

USTRUCT()
struct FBlockFaceVisibility
{
	GENERATED_BODY()
	
public:
	FBlockFaceVisibility() :
		Top(false), Bottom(false), North(false), East(false), South(false), West(false) 
	{}
	FBlockFaceVisibility(bool InTop, bool InBottom, bool InNorth, bool InEast, bool InSouth, bool InWest) :
		Top(InTop), Bottom(InBottom), North(InNorth), East(InEast), South(InSouth), West(InWest)
	{
		Faces = {South, North, West, East, Top, Bottom};
	}

	
	bool Top = false;
	bool Bottom = false;
	bool North = false;
	bool East = false;
	bool South = false;
	bool West = false;

	TArray<bool> Faces;
};

struct FBlockMesh
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<struct FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;
};

UCLASS()
class RATCRAFT_API ARCBlock : public AActor, public IRCInteractable
{
	GENERATED_BODY()
	
public:	
	/**
	 * 
	 */
	ARCBlock();

	void Init(class URCDataAssetBlock* DataAsset, const EBlockType BlockTypeToSpawn, const FVector& GridCoords, const struct FBlockFaceVisibility InBlockFaceVisibility);

	void UpdateMiningProgress();
	virtual void OnInteract() override;
	virtual void EndInteract() override;

	bool IsMining() const { return bIsMining; }

	FVector GetGridCoordinates() const { return GridCoordinates; };
	
private:
	void StartMining();
	void StopMining();
	void OnBlockMined();

	void CreateMesh(const EBlockType BlockTypeToSpawn);
	void ConfigureBlock(const EBlockType BlockTypeToSpawn);

	FColor GetBlockColorFromBlockType(const EBlockType BlockTypeToSpawn);

	UPROPERTY(VisibleAnywhere)
	class UProceduralMeshComponent* ProceduralMesh;
	
	UPROPERTY()
	class URCDataAssetBlock* DataAssetBlock;
	
	FVector GridCoordinates;
	float BlockLength = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	EBlockType BlockType = EBlockType::Air;
	
	struct FBlockFaceVisibility BlockFaceVisibility;
	//Mining
	FTimerHandle MiningTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	float MiningUpdateInterval;
	
	float CurrentMinedTime;
	bool bIsMining;

	bool bIsMinable = false;
};
