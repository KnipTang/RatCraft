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
	float GetWorldChunckSize() const { return ChunckSize * BlockSize; }
	
	static const URCWorldSettings* GetSettings() { return GetDefault<URCWorldSettings>(); }
	
	UPROPERTY(Config, EditDefaultsOnly)
	int ChunckSize = 16;
	UPROPERTY(Config, EditDefaultsOnly)
	int ChunckHeight = 50;
	UPROPERTY(Config, EditDefaultsOnly)
	float PerlinNoiseScale = 0.1f;

	UPROPERTY(Config, EditDefaultsOnly)
	int SnowLevel = 50.f;
	UPROPERTY(Config, EditDefaultsOnly)
	int RockLevel = 5.f;

	UPROPERTY(Config, EditDefaultsOnly)
	float BlockSize = 100.f;
	
	UPROPERTY(Config, EditDefaultsOnly)
	int RenderDistance = 16;

	mutable int Seed = 1;
};
