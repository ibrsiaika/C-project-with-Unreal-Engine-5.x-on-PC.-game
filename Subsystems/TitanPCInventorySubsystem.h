#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "TitanPCInventorySubsystem.generated.h"

USTRUCT(BlueprintType)
struct TITANPC_API FTitanPCItemData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TSoftObjectPtr<UTexture2D> ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FGameplayTag ItemType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MaxStackSize = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    bool bIsConsumable = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float ItemValue = 0.0f;

    FTitanPCItemData()
    {
        ItemName = FText::FromString("Default Item");
        ItemDescription = FText::FromString("A basic item");
        MaxStackSize = 1;
        bIsConsumable = false;
        ItemValue = 0.0f;
    }
};

USTRUCT(BlueprintType)
struct TITANPC_API FTitanPCInventoryItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 Quantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    FGameplayTagContainer ItemTags;

    FTitanPCInventoryItem()
    {
        ItemID = NAME_None;
        Quantity = 1;
    }

    FTitanPCInventoryItem(FName InItemID, int32 InQuantity = 1)
    {
        ItemID = InItemID;
        Quantity = InQuantity;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryUpdated, FName, ItemID, int32, NewQuantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, FTitanPCInventoryItem, AddedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, FTitanPCInventoryItem, RemovedItem);

UCLASS()
class TITANPC_API UTitanPCInventorySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Inventory Management
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(FName ItemID, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(FName ItemID, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemQuantity(FName ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool HasItem(FName ItemID, int32 RequiredQuantity = 1) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FTitanPCInventoryItem> GetAllItems() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FTitanPCInventoryItem> GetItemsByTag(FGameplayTag Tag) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ClearInventory();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UseConsumableItem(FName ItemID);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    FTitanPCItemData GetItemData(FName ItemID) const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryUpdated OnInventoryUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnItemAdded OnItemAdded;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnItemRemoved OnItemRemoved;

protected:
    void LoadItemDatabase();
    bool CanAddItem(FName ItemID, int32 Quantity) const;
    void BroadcastInventoryUpdate(FName ItemID, int32 NewQuantity);

private:
    UPROPERTY()
    TMap<FName, FTitanPCInventoryItem> InventoryItems;

    UPROPERTY(EditDefaultsOnly, Category = "Database")
    TSoftObjectPtr<UDataTable> ItemDataTable;

    UPROPERTY()
    UDataTable* LoadedItemDataTable;

    int32 MaxInventorySize = 100;
    int32 CurrentInventorySize = 0;
};