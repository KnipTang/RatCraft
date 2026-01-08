// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RC_DataAssetBlockInventory.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API URC_DataAssetBlockInventory : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetInventoryItemAssetType();
	
	UTexture2D* GetIcon() const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItem")
	TSoftObjectPtr<UTexture2D> Icon;
};
