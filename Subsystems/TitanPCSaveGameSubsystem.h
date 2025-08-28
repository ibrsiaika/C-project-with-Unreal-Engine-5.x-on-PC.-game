#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/SaveGame.h"
#include "TitanPCSaveGameSubsystem.generated.h"

USTRUCT(BlueprintType)
struct TITANPC_API FTitanPCPlayerSaveData
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    FVector PlayerLocation;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    FRotator PlayerRotation;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    float PlayerHealth = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    float PlayerStamina = 100.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    int32 PlayerLevel = 1;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Player")
    float ExperiencePoints = 0.0f;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Inventory")
    TMap<FName, int32> InventoryData;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Progress")
    TArray<FName> CompletedQuests;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Settings")
    TMap<FString, FString> GameSettings;

    FTitanPCPlayerSaveData()
    {
        PlayerLocation = FVector::ZeroVector;
        PlayerRotation = FRotator::ZeroRotator;
        PlayerHealth = 100.0f;
        PlayerStamina = 100.0f;
        PlayerLevel = 1;
        ExperiencePoints = 0.0f;
    }
};

UCLASS()
class TITANPC_API UTitanPCGameSave : public USaveGame
{
    GENERATED_BODY()

public:
    UTitanPCGameSave();

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Data")
    FTitanPCPlayerSaveData PlayerData;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Data")
    FDateTime SaveTimestamp;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Data")
    FString SaveSlotName;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Data")
    int32 SaveVersion = 1;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Data")
    float TotalPlayTime = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveCompleted, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadCompleted, bool, bSuccess);

UCLASS()
class TITANPC_API UTitanPCSaveGameSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Save/Load Operations
    UFUNCTION(BlueprintCallable, Category = "Save System")
    void SaveGameData(const FString& SlotName = TEXT("DefaultSave"));

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void LoadGameData(const FString& SlotName = TEXT("DefaultSave"));

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool DoesSaveGameExist(const FString& SlotName = TEXT("DefaultSave"));

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void DeleteSaveGame(const FString& SlotName = TEXT("DefaultSave"));

    UFUNCTION(BlueprintCallable, Category = "Save System")
    TArray<FString> GetAllSaveSlots();

    // Auto-save functionality
    UFUNCTION(BlueprintCallable, Category = "Save System")
    void EnableAutoSave(float IntervalSeconds = 300.0f); // 5 minutes default

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void DisableAutoSave();

    // Data Access
    UFUNCTION(BlueprintPure, Category = "Save System")
    FTitanPCPlayerSaveData GetCurrentSaveData() const;

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void UpdatePlayerData(const FTitanPCPlayerSaveData& NewData);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    void AddCompletedQuest(FName QuestID);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool IsQuestCompleted(FName QuestID) const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Save System")
    FOnSaveCompleted OnSaveCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Save System")
    FOnLoadCompleted OnLoadCompleted;

protected:
    void CollectCurrentGameData();
    void ApplyLoadedGameData();
    void PerformAutoSave();

private:
    UPROPERTY()
    UTitanPCGameSave* CurrentSaveGame;

    FString CurrentSlotName = TEXT("DefaultSave");
    FTimerHandle AutoSaveTimer;
    float AutoSaveInterval = 300.0f; // 5 minutes
    bool bAutoSaveEnabled = false;
    float SessionStartTime = 0.0f;
};