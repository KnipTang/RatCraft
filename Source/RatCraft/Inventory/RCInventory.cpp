// Fill out your copyright notice in the Description page of Project Settings.


#include "RCInventory.h"

#include "RCInventoryItem.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"

void URCInventory::Init()
{
	AirBlockTypeID = static_cast<uint8>(EBlockType::Air);
	for (uint8 i = 0; i < InventoryCapacity; i++)
	{
		URCInventoryItem* Item = NewObject<URCInventoryItem>(this);
		Item->Init(
			AirBlockTypeID, 0, i
			);
		OnItemAdded.Broadcast(Item);
		InventoryStorage.Emplace(Item);
	}
}

void URCInventory::AddItem(const uint8 BlockTypeID)
{
	URCInventoryItem* Item = GetItem(BlockTypeID);

	if (Item->Count == InvalidCount)
		return;
	
	if (Item->Count < MaxStackSize)
		Item->Count += 1;

	OnItemAdded.Broadcast(Item);
}

void URCInventory::RemoveItem()
{
	URCInventoryItem* Item = InventoryStorage[CurrentlySelectedSlot];

	Item->Count--;
	
	OnItemRemoved.Broadcast(Item);
	
	if (Item->Count <= 0)
	{
		Item->BlockTypeID = AirBlockTypeID;
	}
}

void URCInventory::UpdateSelectedSlot(const int8 UpdateValue)
{
	CurrentlySelectedSlot += UpdateValue;
	if (CurrentlySelectedSlot > InventoryCapacity - 1 && CurrentlySelectedSlot < MAX_int8/2)
		CurrentlySelectedSlot = 0;
	CurrentlySelectedSlot = FMath::Clamp(CurrentlySelectedSlot, 0, InventoryCapacity - 1);

	OnUpdateSelectedSlot.Broadcast(CurrentlySelectedSlot);
}

uint8 URCInventory::GetCurrentlyHoldingBlockTypeID()
{
	return InventoryStorage[CurrentlySelectedSlot]->BlockTypeID;
}

URCInventoryItem* URCInventory::GetItem(const uint8 BlockTypeID)
{
	for (uint8 i = 0; i < InventoryStorage.Num(); i++)
	{
		if (InventoryStorage[i]->BlockTypeID == BlockTypeID)
			return InventoryStorage[i];
	}
	for (uint8 i = 0; i < InventoryStorage.Num(); i++)
	{
		if (InventoryStorage[i]->BlockTypeID == AirBlockTypeID)
		{
			InventoryStorage[i]->BlockTypeID = BlockTypeID;
			return InventoryStorage[i];
		}
	}

	return nullptr;
}
