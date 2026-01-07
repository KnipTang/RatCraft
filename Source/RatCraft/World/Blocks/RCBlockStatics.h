// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RatCraft/World/RCWorldSettings.h"
#include "RCBlockStatics.generated.h"

UENUM(BlueprintType)
enum class EBlockType : uint8
{
	Grass 						UMETA(DisplayName = "Grass"),
	Dirt						UMETA(DisplayName = "Dirt"),
	Stone						UMETA(DisplayName = "Stone"),
	Snow						UMETA(DisplayName = "Snow"),
	Air							UMETA(DisplayName = "Air"),
};

USTRUCT()
struct FBlockFaceVisibility
{
	GENERATED_BODY()
	
public:
	FBlockFaceVisibility() :
		South(false), North(false), West(false), East(false), Top(false), Bottom(false)
	{
		Faces = {South, North, West, East, Top, Bottom};
	}
	FBlockFaceVisibility(bool InSouth, bool InNorth, bool InWest, bool InEast, bool InTop, bool InBottom) :
		South(InSouth), North(InNorth), West(InWest), East(InEast), Top(InTop), Bottom(InBottom)
	{
		Faces = {South, North, West, East, Top, Bottom};
	}

	
	bool South = false;
	bool North = false;
	bool West = false;
	bool East = false;
	bool Top = false;
	bool Bottom = false;

	TArray<bool> Faces;
};

static EBlockType GetBlockTypeFromHeight(const URCWorldSettings* WorldSettings, const int TerrainHeight, const int BlockHeight)
{
	const int RandomOffset = FMath::RandRange(0, 2);

	const int ChunckHeight = WorldSettings->ChunckHeight;
	const int RockLevel = WorldSettings->RockLevel;
	const int SnowLevel = WorldSettings->SnowLevel;
	
	if (BlockHeight > TerrainHeight)
		return EBlockType::Air;
	else if (BlockHeight == TerrainHeight && BlockHeight < ChunckHeight - (SnowLevel + RandomOffset))
		return EBlockType::Grass;
	else if (BlockHeight >= TerrainHeight - (RockLevel - RandomOffset) && BlockHeight < ChunckHeight - (SnowLevel + RandomOffset))
		return EBlockType::Dirt;
	else if (BlockHeight <= TerrainHeight - (RockLevel - RandomOffset))
		return EBlockType::Stone;
	else if (BlockHeight >= ChunckHeight - (SnowLevel + RandomOffset))
		return EBlockType::Snow;
	
	return EBlockType::Air;
}

static TArray<TArray<int32>> Faces = {
	{3, 2, 1, 0}, // South
	{6, 7, 4, 5}, // North
	{7, 3, 0, 4}, // West
	{2, 6, 5, 1}, // East
	{7, 6, 2, 3}, // Top
	{0, 1, 5, 4}  // Bottom
};
static TArray<FVector> FaceNormals = {
	FVector(0, -1, 0),  // South
	FVector(0, 1, 0),   // North
	FVector(-1, 0, 0),  // West
	FVector(1, 0, 0),   // East
	FVector(0, 0, 1),   // Top
	FVector(0, 0, -1)   // Bottom
};

static const TArray<FVector>& GetCubeVertices(const float HalfBlockSize)
{
	static TArray<FVector> CubeVertices;

	static bool bInitialized = false;
    
	if (!bInitialized)
	{
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
		bInitialized = true;
	}
    
	return CubeVertices;
}
