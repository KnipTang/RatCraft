// Fill out your copyright notice in the Description page of Project Settings.


#include "RCInventory.h"

#include "RCInventoryItem.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"

void URCInventory::Init()
{
	for (uint8 i = 0; i < InventoryCapacity; i++)
	{
		URCInventoryItem* Item = NewObject<URCInventoryItem>(this);
		Item->Init(
			EBlockType::Air, 0, i
			);
		OnItemAdded.Broadcast(Item);
		InventoryStorage.Emplace(Item);
	}
}

void URCInventory::AddItem(const EBlockType BlockType)
{
	URCInventoryItem* Item = GetItem(BlockType);

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
		Item->BlockType = EBlockType::Air;
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

EBlockType URCInventory::GetCurrentlyHoldingBlockType()
{
	return InventoryStorage[CurrentlySelectedSlot]->BlockType;
}

URCInventoryItem* URCInventory::GetItem(const EBlockType BlockType)
{
	for (URCInventoryItem* Item : InventoryStorage)
	{
		if (Item->BlockType == BlockType)
			return Item;
	}
	for (URCInventoryItem* Item : InventoryStorage)
	{
		if (Item->BlockType == BlockTypesCount)
		{
			Item->BlockType = BlockType;
			return Item;
		}
	}

	return nullptr;
}
