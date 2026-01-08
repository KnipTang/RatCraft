// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "RatCraft/Inventory/RCInventoryItem.h"
#include "RatCraft/Inventory/RC_DataAssetBlockInventory.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EmptySlot();
}

void UItemWidget::UpdateInventoryItem(const URCInventoryItem* InInventoryItem, const URC_DataAssetBlockInventory* DataAsset)
{
	InventoryItem = InInventoryItem;
	if (!InventoryItem)
	{
		EmptySlot();
		return;
	}

	SetIcon(DataAsset->GetIcon());
}

void UItemWidget::SetIcon(UTexture2D* IconTexture)
{
	ItemIcon->SetBrushFromTexture(IconTexture);
}

void UItemWidget::ChangeSelected(bool bSelected) const
{
	bSelected ? Background->SetBrushColor(ColorBackgroundSelected) : Background->SetBrushColor(ColorBackgroundUnselected);
}

bool UItemWidget::IsEmpty() const
{
	return !InventoryItem || InventoryItem->BlockType == EBlockType::Air;
}

void UItemWidget::EmptySlot()
{
	InventoryItem = nullptr;
	SetIcon(EmptyTexture);
}

void UItemWidget::SetSlotNumber(int NewSlotNumber)
{
	SlotNumber = NewSlotNumber;
}
