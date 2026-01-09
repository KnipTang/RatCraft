// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "ItemWidget.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "RatCraft/Inventory/RCInventory.h"
#include "RatCraft/Inventory/RCInventoryItem.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		PlayerInventory = OwnerPawn->GetComponentByClass<URCInventory>();
		if (PlayerInventory)
		{
			PlayerInventory->OnItemAdded.AddUObject(this, &UInventoryWidget::ItemAdded);
			PlayerInventory->OnItemRemoved.AddUObject(this, &UInventoryWidget::ItemRemove);

			PlayerInventory->OnUpdateSelectedSlot.AddUObject(this, &UInventoryWidget::UpdateSelectedSlot);
			
			InventoryCapacity = PlayerInventory->GetInventoryCapacity();
			MaxStackSize = PlayerInventory->GetMaxStackSize();
			
			ItemsContainer->ClearChildren();

			for (uint8 i = 0; i < InventoryCapacity; ++i)
			{
				if (UItemWidget* NewEmptyWidget = CreateWidget<UItemWidget>(GetOwningPlayer(), ItemWidgetClass))
				{
					UWrapBoxSlot* NewItemSlot = ItemsContainer->AddChildToWrapBox(NewEmptyWidget);
					NewItemSlot->SetPadding(FMargin(2.f));
					ItemWidgets.Add(NewEmptyWidget);
				}
			}
			SelectedItemSlot = 0;
			ItemWidgets[SelectedItemSlot]->ChangeSelected(true);
		}
	}
}

void UInventoryWidget::ItemAdded(const struct FRCInventoryItem& InventoryItem)
{
	const EBlockType BlockType = InventoryItem.BlockType;
	
	if (BlockType == EBlockType::Air)
		return;
	
	UItemWidget* ItemWidget{};

	for (TPair<uint8 /*Slot*/, FRCInventoryItem>& Item : PopulatedItems)
	{
		if (Item.Value.BlockType == BlockType && Item.Value.Count != MaxStackSize)
		{
			Item.Value.Count = InventoryItem.Count;
			ItemWidget = ItemWidgets[Item.Key];
			ItemWidget->UpdateInventoryItem(InventoryItem, InventoryItemsData.FindChecked(BlockType));
			return;
		}
	}

	if (const uint8 FreeSlot = GetNextAvailableSlot(); FreeSlot != UINT8_MAX)
	{
		ItemWidget = ItemWidgets[FreeSlot];
		const FRCInventoryItem NewInventoryItem = FRCInventoryItem{InventoryItem.BlockType, InventoryItem.Count};
		PopulatedItems.Add(FreeSlot, NewInventoryItem);
		ItemWidget->UpdateInventoryItem(InventoryItem, InventoryItemsData.FindChecked(BlockType));
	}
}

void UInventoryWidget::ItemRemove(const struct FRCInventoryItem& InventoryItem)
{
   	ItemWidgets[SelectedItemSlot]->UpdateInventoryItem(InventoryItem, InventoryItemsData.FindChecked(InventoryItem.BlockType));
	if (PopulatedItems.Contains(SelectedItemSlot))
	{
		if (InventoryItem.BlockType == EBlockType::Air)
			PopulatedItems[SelectedItemSlot] = FRCInventoryItem{};
		PopulatedItems[SelectedItemSlot].Count = InventoryItem.Count;
	}
}

void UInventoryWidget::UpdateSelectedSlot(const uint8 SelectedSlot)
{
	if (SelectedSlot < 0 || SelectedSlot >= InventoryCapacity)
		return;

	ItemWidgets[SelectedItemSlot]->ChangeSelected(false);
	
	SelectedItemSlot = SelectedSlot;
	UItemWidget* ItemWidget = ItemWidgets[SelectedSlot];
	if (!ItemWidget)
		return;
	
	ItemWidget->ChangeSelected(true);
}

uint8 UInventoryWidget::GetNextAvailableSlot() const
{
	for (uint8 i = 0; i < InventoryCapacity; ++i)
	{
		if (ItemWidgets[i]->IsEmpty())
			return i;
	}
	return UINT8_MAX;
}
