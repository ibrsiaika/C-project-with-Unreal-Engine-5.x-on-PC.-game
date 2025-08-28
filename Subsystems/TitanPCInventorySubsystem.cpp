#include "TitanPCInventorySubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"

void UTitanPCInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    LoadItemDatabase();
    UE_LOG(LogTemp, Warning, TEXT("TitanPC Inventory Subsystem Initialized"));
}

void UTitanPCInventorySubsystem::Deinitialize()
{
    InventoryItems.Empty();
    Super::Deinitialize();
}

void UTitanPCInventorySubsystem::LoadItemDatabase()
{
    // In a real game, this would load from a data table asset
    // For now, we'll create some default items programmatically
    
    // This would typically be set in the editor or loaded from configuration
    if (ItemDataTable.IsValid())
    {
        LoadedItemDataTable = ItemDataTable.LoadSynchronous();
    }
}

bool UTitanPCInventorySubsystem::AddItem(FName ItemID, int32 Quantity)
{
    if (ItemID == NAME_None || Quantity <= 0)
    {
        return false;
    }

    if (!CanAddItem(ItemID, Quantity))
    {
        return false;
    }

    FTitanPCInventoryItem* ExistingItem = InventoryItems.Find(ItemID);
    if (ExistingItem)
    {
        // Stack with existing item
        FTitanPCItemData ItemData = GetItemData(ItemID);
        int32 NewQuantity = ExistingItem->Quantity + Quantity;
        
        if (NewQuantity <= ItemData.MaxStackSize)
        {
            ExistingItem->Quantity = NewQuantity;
            BroadcastInventoryUpdate(ItemID, NewQuantity);
            return true;
        }
        else
        {
            // Can't stack beyond max stack size
            int32 CanAdd = ItemData.MaxStackSize - ExistingItem->Quantity;
            if (CanAdd > 0)
            {
                ExistingItem->Quantity = ItemData.MaxStackSize;
                BroadcastInventoryUpdate(ItemID, ItemData.MaxStackSize);
                // Try to add remaining quantity as new stack (simplified for now)
                return CanAdd == Quantity;
            }
            return false;
        }
    }
    else
    {
        // Add new item
        FTitanPCInventoryItem NewItem(ItemID, Quantity);
        InventoryItems.Add(ItemID, NewItem);
        CurrentInventorySize++;
        
        OnItemAdded.Broadcast(NewItem);
        BroadcastInventoryUpdate(ItemID, Quantity);
        return true;
    }
}

bool UTitanPCInventorySubsystem::RemoveItem(FName ItemID, int32 Quantity)
{
    if (ItemID == NAME_None || Quantity <= 0)
    {
        return false;
    }

    FTitanPCInventoryItem* ExistingItem = InventoryItems.Find(ItemID);
    if (!ExistingItem || ExistingItem->Quantity < Quantity)
    {
        return false;
    }

    ExistingItem->Quantity -= Quantity;
    
    if (ExistingItem->Quantity <= 0)
    {
        FTitanPCInventoryItem RemovedItem = *ExistingItem;
        InventoryItems.Remove(ItemID);
        CurrentInventorySize--;
        OnItemRemoved.Broadcast(RemovedItem);
        BroadcastInventoryUpdate(ItemID, 0);
    }
    else
    {
        BroadcastInventoryUpdate(ItemID, ExistingItem->Quantity);
    }

    return true;
}

int32 UTitanPCInventorySubsystem::GetItemQuantity(FName ItemID) const
{
    const FTitanPCInventoryItem* Item = InventoryItems.Find(ItemID);
    return Item ? Item->Quantity : 0;
}

bool UTitanPCInventorySubsystem::HasItem(FName ItemID, int32 RequiredQuantity) const
{
    return GetItemQuantity(ItemID) >= RequiredQuantity;
}

TArray<FTitanPCInventoryItem> UTitanPCInventorySubsystem::GetAllItems() const
{
    TArray<FTitanPCInventoryItem> Items;
    InventoryItems.GenerateValueArray(Items);
    return Items;
}

TArray<FTitanPCInventoryItem> UTitanPCInventorySubsystem::GetItemsByTag(FGameplayTag Tag) const
{
    TArray<FTitanPCInventoryItem> FilteredItems;
    
    for (const auto& ItemPair : InventoryItems)
    {
        const FTitanPCInventoryItem& Item = ItemPair.Value;
        if (Item.ItemTags.HasTag(Tag))
        {
            FilteredItems.Add(Item);
        }
    }
    
    return FilteredItems;
}

void UTitanPCInventorySubsystem::ClearInventory()
{
    InventoryItems.Empty();
    CurrentInventorySize = 0;
    OnInventoryUpdated.Broadcast(NAME_None, 0); // Broadcast general inventory clear
}

bool UTitanPCInventorySubsystem::UseConsumableItem(FName ItemID)
{
    FTitanPCItemData ItemData = GetItemData(ItemID);
    if (!ItemData.bIsConsumable)
    {
        return false;
    }

    if (HasItem(ItemID, 1))
    {
        RemoveItem(ItemID, 1);
        // Here you would trigger the consumable effect
        UE_LOG(LogTemp, Warning, TEXT("Used consumable item: %s"), *ItemID.ToString());
        return true;
    }

    return false;
}

FTitanPCItemData UTitanPCInventorySubsystem::GetItemData(FName ItemID) const
{
    if (LoadedItemDataTable)
    {
        if (FTitanPCItemData* Data = LoadedItemDataTable->FindRow<FTitanPCItemData>(ItemID, TEXT("GetItemData")))
        {
            return *Data;
        }
    }
    
    // Return default data if not found
    FTitanPCItemData DefaultData;
    DefaultData.ItemName = FText::FromName(ItemID);
    return DefaultData;
}

bool UTitanPCInventorySubsystem::CanAddItem(FName ItemID, int32 Quantity) const
{
    // Check if we have room in inventory
    const FTitanPCInventoryItem* ExistingItem = InventoryItems.Find(ItemID);
    if (!ExistingItem)
    {
        // New item, check if we have space
        return CurrentInventorySize < MaxInventorySize;
    }
    else
    {
        // Existing item, check if we can stack
        FTitanPCItemData ItemData = GetItemData(ItemID);
        return (ExistingItem->Quantity + Quantity) <= ItemData.MaxStackSize;
    }
}

void UTitanPCInventorySubsystem::BroadcastInventoryUpdate(FName ItemID, int32 NewQuantity)
{
    OnInventoryUpdated.Broadcast(ItemID, NewQuantity);
}