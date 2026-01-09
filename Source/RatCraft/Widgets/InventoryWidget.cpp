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
			
			const uint8 Capacity = PlayerInventory->GetInventoryCapacity();

			ItemsContainer->ClearChildren();

			for (int i = 0; i < Capacity; ++i)
			{
				if (UItemWidget* NewEmptyWidget = CreateWidget<UItemWidget>(GetOwningPlayer(), ItemWidgetClass))
				{
					UWrapBoxSlot* NewItemSlot = ItemsContainer->AddChildToWrapBox(NewEmptyWidget);
					NewItemSlot->SetPadding(FMargin(2.f));
					ItemWidgets.Add(NewEmptyWidget);
				}
			}
			SelectedItemWidget = ItemWidgets[0];
			SelectedItemWidget->ChangeSelected(true);
		}
	}
}

void UInventoryWidget::ItemAdded(const struct FRCInventoryItem& InventoryItem)
{
	const EBlockType BlockType = InventoryItem.BlockType;
	UItemWidget* ItemWidget{};
	if (!PopulatedItemEntryWidgets.Contains(BlockType))
	{
		if (UItemWidget* NextAvailableSlot = GetNextAvailableSlot())
		{
			PopulatedItemEntryWidgets.Add(BlockType, NextAvailableSlot);
			ItemWidget = NextAvailableSlot;
		}
	}
	else
	{
		ItemWidget = PopulatedItemEntryWidgets[BlockType];
	}
	ItemWidget->UpdateInventoryItem(InventoryItem, InventoryItemsData.FindChecked(BlockType));
}

void UInventoryWidget::ItemRemove(const struct FRCInventoryItem& InventoryItem)
{
	SelectedItemWidget->UpdateInventoryItem(InventoryItem, InventoryItemsData.FindChecked(InventoryItem.BlockType));
}

void UInventoryWidget::UpdateSelectedSlot(const uint8 SelectedSlot)
{
	if (SelectedSlot < 0 || SelectedSlot >= ItemWidgets.Num())
		return;
	
	UItemWidget* ItemWidget = ItemWidgets[SelectedSlot];
	if (SelectedItemWidget == ItemWidget)
		return;
	if (!SelectedItemWidget)
		return;
	
	SelectedItemWidget->ChangeSelected(false);
	SelectedItemWidget = ItemWidget;
	ItemWidget->ChangeSelected(true);
}

UItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
	for (UItemWidget* Widget : ItemWidgets)
	{
		if (Widget->IsEmpty())
		{
			return Widget;
		}
	}
	
	return nullptr;
}
