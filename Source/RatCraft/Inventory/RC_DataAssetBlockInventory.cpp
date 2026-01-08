// Fill out your copyright notice in the Description page of Project Settings.


#include "RC_DataAssetBlockInventory.h"

FPrimaryAssetId URC_DataAssetBlockInventory::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetInventoryItemAssetType(), GetFName());
}

FPrimaryAssetType URC_DataAssetBlockInventory::GetInventoryItemAssetType()
{
	return FPrimaryAssetType("InventoryItem");
}

UTexture2D* URC_DataAssetBlockInventory::GetIcon() const
{
	return Icon.LoadSynchronous();
}
