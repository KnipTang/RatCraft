// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RatCraft/World/RCWorldSettings.h"
#include "RCBlock.generated.h"

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

static EBlockType GetBlockTypeFromHeight(const int TerrainHeight, const int BlockHeight)
{
	const URCWorldSettings* WorldSettings = URCWorldSettings::GetSettings();
	int RandomOffset = FMath::RandRange(0, 2);
	
	if (BlockHeight > TerrainHeight)
		return EBlockType::Air;
	else if (BlockHeight == TerrainHeight && BlockHeight < WorldSettings->ChunckHeight - (WorldSettings->SnowLevel + RandomOffset))
		return EBlockType::Grass;
	else if (BlockHeight >= TerrainHeight - (WorldSettings->RockLevel - RandomOffset) && BlockHeight < WorldSettings->ChunckHeight - (WorldSettings->SnowLevel + RandomOffset))
		return EBlockType::Dirt;
	else if (BlockHeight <= TerrainHeight - (WorldSettings->RockLevel - RandomOffset))
		return EBlockType::Stone;
	else if (BlockHeight >= WorldSettings->ChunckHeight - (WorldSettings->SnowLevel + RandomOffset))
		return EBlockType::Snow;
	
	return EBlockType::Air;
}
