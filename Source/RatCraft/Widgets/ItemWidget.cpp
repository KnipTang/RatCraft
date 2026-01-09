// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "RatCraft/Inventory/RCInventoryItem.h"
#include "RatCraft/Inventory/RC_DataAssetBlockInventory.h"
#include "RatCraft/World/Blocks/RCBlockStatics.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIcon(nullptr);
	SetCount(0);
	ChangeSelected(false);
	InventoryItem = FRCInventoryItem{};
	EmptySlot();
}

void UItemWidget::UpdateInventoryItem(const struct FRCInventoryItem& InInventoryItem,
	const class URC_DataAssetBlockInventory* DataAsset)
{
	InventoryItem = InInventoryItem;
	if (IsEmpty())
	{
		EmptySlot();
		return;
	}

	SetIcon(DataAsset->GetIcon());
	SetCount(InventoryItem.Count);
}

void UItemWidget::ChangeSelected(const bool bSelected) const
{
	bSelected ? Background->SetBrushColor(ColorBackgroundSelected) : Background->SetBrushColor(ColorBackgroundUnselected);
}

bool UItemWidget::IsEmpty() const
{
	return InventoryItem.BlockType == EBlockType::Air;
}

void UItemWidget::EmptySlot()
{
	InventoryItem.BlockType = EBlockType::Air;
	SetIcon(EmptyTexture);
	SetCount(0);
}

void UItemWidget::SetIcon(UTexture2D* IconTexture) const
{
	IconTexture == nullptr ? ItemIcon->SetVisibility(ESlateVisibility::Hidden) : ItemIcon->SetVisibility(ESlateVisibility::Visible);
	
	ItemIcon->SetBrushFromTexture(IconTexture);
}

void UItemWidget::SetCount(const uint8 Count) const
{
	Count <= 0 ? ItemCount->SetVisibility(ESlateVisibility::Hidden) : ItemCount->SetVisibility(ESlateVisibility::Visible);
	
	ItemCount->SetText(FText::AsNumber(Count));
}
