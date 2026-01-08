// Fill out your copyright notice in the Description page of Project Settings.


#include "RCAssetManager.h"

#include "RatCraft/Inventory/RC_DataAssetBlockInventory.h"

URCAssetManager& URCAssetManager::Get()
{
	if (URCAssetManager* Singleton = Cast<URCAssetManager>(GEngine->AssetManager.Get()))
		return *Singleton;

	UE_LOG(LogLoad, Fatal, TEXT("Asset Manger isn't of type TfsAssetManager"));
	return *NewObject<URCAssetManager>();
}

void URCAssetManager::LoadInventoryItems(const FStreamableDelegate& LoadFinishCallBack)
{
	LoadPrimaryAssetsWithType(URC_DataAssetBlockInventory::GetInventoryItemAssetType(), TArray<FName>(), FStreamableDelegate::CreateUObject(this, &URCAssetManager::InventoryItemsFinishedLoading, LoadFinishCallBack));
}

bool URCAssetManager::GetLoadedInventoryItems(TArray<const class URC_DataAssetBlockInventory*>& OutItems)
{
	if (bAllInventoryItemsFinishedLoading)
	{
		OutItems = AllLoadedItems;
		return bAllInventoryItemsFinishedLoading;
	}
	
	TArray<UObject*> LoadedObjects;
	bAllInventoryItemsFinishedLoading = GetPrimaryAssetObjectList(URC_DataAssetBlockInventory::GetInventoryItemAssetType(), LoadedObjects);

	if (bAllInventoryItemsFinishedLoading)
	{
		for (UObject* LoadedObject : LoadedObjects)
			AllLoadedItems.Add(Cast<URC_DataAssetBlockInventory>(LoadedObject));
	}
	OutItems = AllLoadedItems;
	return bAllInventoryItemsFinishedLoading;
}

void URCAssetManager::InventoryItemsFinishedLoading(FStreamableDelegate CallBack)
{
	CallBack.ExecuteIfBound();
}
