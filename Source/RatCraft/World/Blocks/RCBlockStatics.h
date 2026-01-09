// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RatCraft/World/RCWorldSettings.h"
#include "RCBlockStatics.generated.h"

UENUM(BlueprintType)
enum EBlockType : uint8
{
	Grass 						UMETA(DisplayName = "Grass"),
	Dirt						UMETA(DisplayName = "Dirt"),
	Stone						UMETA(DisplayName = "Stone"),
	Snow						UMETA(DisplayName = "Snow"),
	Air							UMETA(DisplayName = "Air"),
};

template<typename TEnum>
FORCEINLINE uint8 ToUInt8(TEnum EnumValue)
{
    return static_cast<uint8>(EnumValue);
}

template<typename TEnum>
FORCEINLINE TEnum ToTEnum(uint8 Value)
{
    return static_cast<TEnum>(Value);
}

static uint8 BlockTypesCount = ToUInt8(EBlockType::Air);

static TArray<TArray<uint8>> Faces = {
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
