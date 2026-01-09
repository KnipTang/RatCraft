// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RCWorldSettings.generated.h"
/**
 * 
 */
UCLASS( Config = "Game" )
class RATCRAFT_API URCWorldSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	URCWorldSettings();
	
	float GetHalfBlockSize() const { return BlockSize / 2.f; }
	float GetWorldChunkSize() const { return ChunkSize * BlockSize; }
	
	static const URCWorldSettings* GetSettings() { return GetDefault<URCWorldSettings>(); }
	
	UPROPERTY(Config, EditDefaultsOnly)
	uint8 ChunkSize = 16;
	UPROPERTY(Config, EditDefaultsOnly)
	uint8 ChunkHeight = 20;
	UPROPERTY(Config, EditDefaultsOnly)
	float PerlinNoiseScale = 0.1f;

	UPROPERTY(Config, EditDefaultsOnly)
	uint8 SnowLevel = 5;
	UPROPERTY(Config, EditDefaultsOnly)
	uint8 RockLevel = 3;

	UPROPERTY(Config, EditDefaultsOnly)
	uint8 BlockSize = 100;
	
	UPROPERTY(Config, EditDefaultsOnly)
	uint8 RenderDistance = 16;

	UPROPERTY(Config, EditDefaultsOnly)
	uint8 InitChunksLoadedRange = 5;
	
	mutable uint32 Seed = 1;
	mutable FVector2D SeedOffset;
};
