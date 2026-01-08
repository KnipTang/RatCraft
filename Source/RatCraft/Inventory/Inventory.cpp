// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

UInventory::UInventory()
{
	AirBlockTypeID = static_cast<uint8>(EBlockType::Air);
	for (uint8 i = 0; i < InventoryCapacity; i++)
	{
		InventoryStorage.Emplace(AirBlockTypeID, 0);
	}
}

void UInventory::AddItem(const uint8 BlockTypeID)
{
	int8& ItemCount = GetItemCount(BlockTypeID);

	if (ItemCount == InvalidValue)
		return;
	
	if (ItemCount < MaxStackSize)
		ItemCount += 1;
}

void UInventory::RemoveItem()
{
	FInventoryItem& Item = InventoryStorage[CurrentlySelectedSlot];

	Item.Count--;
	
	if (Item.Count <= 0)
	{
		Item.BlockTypeID = AirBlockTypeID;
	}
}

uint8 UInventory::GetCurrentlyHoldingBlockTypeID()
{
	return InventoryStorage[CurrentlySelectedSlot].BlockTypeID;
}

int8& UInventory::GetItemCount(const uint8 BlockTypeID)
{
	for (uint8 i = 0; i < InventoryStorage.Num(); i++)
	{
		if (InventoryStorage[i].BlockTypeID == BlockTypeID)
			return InventoryStorage[i].Count;
	}
	for (uint8 i = 0; i < InventoryStorage.Num(); i++)
	{
		if (InventoryStorage[i].BlockTypeID == AirBlockTypeID)
		{
			InventoryStorage[i].BlockTypeID = BlockTypeID;
			return InventoryStorage[i].Count;
		}
	}

	return InvalidValue;
}
