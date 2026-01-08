// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "RCAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class RATCRAFT_API URCAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static URCAssetManager& Get();
	void LoadInventoryItems(const FStreamableDelegate& LoadFinishCallBack);
	bool GetLoadedInventoryItems(TArray<const class URC_DataAssetBlockInventory*>& OutItems);
private:
	void InventoryItemsFinishedLoading(FStreamableDelegate CallBack);

	UPROPERTY()
	TArray<const class URC_DataAssetBlockInventory*> AllLoadedItems;
	bool bAllInventoryItemsFinishedLoading;
};
