// Fill out your copyright notice in the Description page of Project Settings.


#include "RCInventory.h"

#include "RCInventoryItem.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"

void URCInventory::Init()
{
	for (uint8 i = 0; i < InventoryCapacity; i++)
	{
		FRCInventoryItem NewItem{
			EBlockType::Air, 0
			};
		OnItemAdded.Broadcast(NewItem);
		InventoryStorage.Emplace(NewItem);
	}
}

void URCInventory::AddItem(const EBlockType BlockType)
{
	FRCInventoryItem* Item = GetItem(BlockType);

	if (Item->BlockType == EBlockType::Air)
		return;
	
	if (Item->Count == InvalidCount)
		return;
	
	if (Item->Count < MaxStackSize)
		Item->Count += 1;

	OnItemAdded.Broadcast(*Item);
}

void URCInventory::RemoveItem()
{
	FRCInventoryItem* Item = &InventoryStorage[CurrentlySelectedSlot];

	Item->Count--;
	
	if (Item->Count <= 0)
	{
		Item->BlockType = EBlockType::Air;
	}
	
	OnItemRemoved.Broadcast(*Item);
}

void URCInventory::UpdateSelectedSlot(const int8 UpdateValue)
{
	CurrentlySelectedSlot += UpdateValue;
	if (CurrentlySelectedSlot > InventoryCapacity - 1 && CurrentlySelectedSlot < MAX_int8/2)
		CurrentlySelectedSlot = 0;
	CurrentlySelectedSlot = FMath::Clamp(CurrentlySelectedSlot, 0, InventoryCapacity - 1);

	OnUpdateSelectedSlot.Broadcast(CurrentlySelectedSlot);
}

EBlockType URCInventory::GetCurrentlyHoldingBlockType()
{
	return InventoryStorage[CurrentlySelectedSlot].BlockType;
}

FRCInventoryItem* URCInventory::GetItem(const EBlockType BlockType)
{
	FRCInventoryItem* FirstEmptySlot{};
    
	for (FRCInventoryItem& Item : InventoryStorage)
	{
		if (Item.BlockType == BlockType && Item.Count != MaxStackSize)
		{
			return &Item;
		}
		
		if (!FirstEmptySlot && Item.BlockType == EBlockType::Air)
		{
			FirstEmptySlot = &Item;
		}
	}
	if (FirstEmptySlot)
	{
		FirstEmptySlot->BlockType = BlockType;
		FirstEmptySlot->Count = 0;
		return FirstEmptySlot;
	}

	return nullptr;
}
