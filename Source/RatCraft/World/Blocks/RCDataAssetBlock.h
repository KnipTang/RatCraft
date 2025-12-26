// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RCDataAssetBlock.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API URCDataAssetBlock : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	float GetMineTime() const { return MineTime; }
	UMaterialInterface* GetMaterial() const { return Material; }
private:
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	UMaterialInterface* Material;
	
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MineTime;
};
