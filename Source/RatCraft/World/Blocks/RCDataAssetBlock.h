// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RCDataAssetBlock.generated.h"

enum class EBlockType : uint8;
/**
 * 
 */
UCLASS()
class RATCRAFT_API URCDataAssetBlock : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	EBlockType GetBlockType() const { return BlockType; }
	
	float GetMineTime() const { return MineTime; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	EBlockType BlockType;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UMaterialInterface* Material;
	
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MineTime;
};
