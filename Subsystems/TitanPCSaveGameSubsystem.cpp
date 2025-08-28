#include "TitanPCSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "TitanPCInventorySubsystem.h"

UTitanPCGameSave::UTitanPCGameSave()
{
    SaveTimestamp = FDateTime::Now();
    SaveSlotName = TEXT("DefaultSave");
    SaveVersion = 1;
    TotalPlayTime = 0.0f;
}

void UTitanPCSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    CurrentSaveGame = nullptr;
    SessionStartTime = FPlatformTime::Seconds();
    
    // Create a default save game object
    CurrentSaveGame = Cast<UTitanPCGameSave>(UGameplayStatics::CreateSaveGameObject(UTitanPCGameSave::StaticClass()));
    
    UE_LOG(LogTemp, Warning, TEXT("TitanPC Save Game Subsystem Initialized"));
}

void UTitanPCSaveGameSubsystem::Deinitialize()
{
    DisableAutoSave();
    CurrentSaveGame = nullptr;
    Super::Deinitialize();
}

void UTitanPCSaveGameSubsystem::SaveGameData(const FString& SlotName)
{
    if (!CurrentSaveGame)
    {
        CurrentSaveGame = Cast<UTitanPCGameSave>(UGameplayStatics::CreateSaveGameObject(UTitanPCGameSave::StaticClass()));
    }

    CurrentSlotName = SlotName;
    CurrentSaveGame->SaveSlotName = SlotName;
    CurrentSaveGame->SaveTimestamp = FDateTime::Now();
    
    // Calculate total play time
    float CurrentTime = FPlatformTime::Seconds();
    CurrentSaveGame->TotalPlayTime += (CurrentTime - SessionStartTime);
    SessionStartTime = CurrentTime;

    // Collect current game state
    CollectCurrentGameData();

    // Perform the save operation
    bool bSaveSuccess = UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
    
    OnSaveCompleted.Broadcast(bSaveSuccess);
    
    if (bSaveSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game saved successfully to slot: %s"), *SlotName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save game to slot: %s"), *SlotName);
    }
}

void UTitanPCSaveGameSubsystem::LoadGameData(const FString& SlotName)
{
    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("Save game does not exist: %s"), *SlotName);
        OnLoadCompleted.Broadcast(false);
        return;
    }

    USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
    UTitanPCGameSave* LoadedTitanSave = Cast<UTitanPCGameSave>(LoadedGame);

    if (!LoadedTitanSave)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load or cast save game: %s"), *SlotName);
        OnLoadCompleted.Broadcast(false);
        return;
    }

    CurrentSaveGame = LoadedTitanSave;
    CurrentSlotName = SlotName;
    SessionStartTime = FPlatformTime::Seconds();

    // Apply the loaded data to the game
    ApplyLoadedGameData();

    OnLoadCompleted.Broadcast(true);
    UE_LOG(LogTemp, Warning, TEXT("Game loaded successfully from slot: %s"), *SlotName);
}

bool UTitanPCSaveGameSubsystem::DoesSaveGameExist(const FString& SlotName)
{
    return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

void UTitanPCSaveGameSubsystem::DeleteSaveGame(const FString& SlotName)
{
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        bool bDeleteSuccess = UGameplayStatics::DeleteGameInSlot(SlotName, 0);
        if (bDeleteSuccess)
        {
            UE_LOG(LogTemp, Warning, TEXT("Deleted save game: %s"), *SlotName);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to delete save game: %s"), *SlotName);
        }
    }
}

TArray<FString> UTitanPCSaveGameSubsystem::GetAllSaveSlots()
{
    TArray<FString> SaveSlots;
    
    // In a full implementation, you would scan the save directory
    // For now, return common save slots
    SaveSlots.Add(TEXT("DefaultSave"));
    SaveSlots.Add(TEXT("QuickSave"));
    SaveSlots.Add(TEXT("AutoSave"));
    
    // Filter to only existing saves
    SaveSlots.RemoveAll([](const FString& Slot) {
        return !UGameplayStatics::DoesSaveGameExist(Slot, 0);
    });
    
    return SaveSlots;
}

void UTitanPCSaveGameSubsystem::EnableAutoSave(float IntervalSeconds)
{
    DisableAutoSave(); // Clear any existing timer
    
    AutoSaveInterval = FMath::Max(IntervalSeconds, 60.0f); // Minimum 1 minute
    bAutoSaveEnabled = true;
    
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            AutoSaveTimer,
            this,
            &UTitanPCSaveGameSubsystem::PerformAutoSave,
            AutoSaveInterval,
            true
        );
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Auto-save enabled with %f second intervals"), AutoSaveInterval);
}

void UTitanPCSaveGameSubsystem::DisableAutoSave()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoSaveTimer);
    }
    
    bAutoSaveEnabled = false;
    UE_LOG(LogTemp, Warning, TEXT("Auto-save disabled"));
}

FTitanPCPlayerSaveData UTitanPCSaveGameSubsystem::GetCurrentSaveData() const
{
    return CurrentSaveGame ? CurrentSaveGame->PlayerData : FTitanPCPlayerSaveData();
}

void UTitanPCSaveGameSubsystem::UpdatePlayerData(const FTitanPCPlayerSaveData& NewData)
{
    if (CurrentSaveGame)
    {
        CurrentSaveGame->PlayerData = NewData;
    }
}

void UTitanPCSaveGameSubsystem::AddCompletedQuest(FName QuestID)
{
    if (CurrentSaveGame && !CurrentSaveGame->PlayerData.CompletedQuests.Contains(QuestID))
    {
        CurrentSaveGame->PlayerData.CompletedQuests.Add(QuestID);
    }
}

bool UTitanPCSaveGameSubsystem::IsQuestCompleted(FName QuestID) const
{
    return CurrentSaveGame ? CurrentSaveGame->PlayerData.CompletedQuests.Contains(QuestID) : false;
}

void UTitanPCSaveGameSubsystem::CollectCurrentGameData()
{
    if (!CurrentSaveGame) return;

    // Get player controller and pawn
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            if (APawn* PlayerPawn = PC->GetPawn())
            {
                // Save player location and rotation
                CurrentSaveGame->PlayerData.PlayerLocation = PlayerPawn->GetActorLocation();
                CurrentSaveGame->PlayerData.PlayerRotation = PlayerPawn->GetActorRotation();
            }
        }
    }

    // Collect inventory data
    if (UTitanPCInventorySubsystem* InventorySystem = GetGameInstance()->GetSubsystem<UTitanPCInventorySubsystem>())
    {
        CurrentSaveGame->PlayerData.InventoryData.Empty();
        TArray<FTitanPCInventoryItem> AllItems = InventorySystem->GetAllItems();
        
        for (const FTitanPCInventoryItem& Item : AllItems)
        {
            CurrentSaveGame->PlayerData.InventoryData.Add(Item.ItemID, Item.Quantity);
        }
    }

    // Here you would collect other game state data like:
    // - Character stats (health, stamina, level, etc.)
    // - Quest progress
    // - World state changes
    // - Settings
}

void UTitanPCSaveGameSubsystem::ApplyLoadedGameData()
{
    if (!CurrentSaveGame) return;

    // Apply player position (would typically be done during level load)
    UE_LOG(LogTemp, Warning, TEXT("Loading player data - Position: %s"), *CurrentSaveGame->PlayerData.PlayerLocation.ToString());

    // Restore inventory
    if (UTitanPCInventorySubsystem* InventorySystem = GetGameInstance()->GetSubsystem<UTitanPCInventorySubsystem>())
    {
        InventorySystem->ClearInventory();
        
        for (const auto& ItemPair : CurrentSaveGame->PlayerData.InventoryData)
        {
            InventorySystem->AddItem(ItemPair.Key, ItemPair.Value);
        }
    }

    // Here you would restore other game state:
    // - Character stats
    // - Quest states
    // - World modifications
    // - Settings
}

void UTitanPCSaveGameSubsystem::PerformAutoSave()
{
    if (bAutoSaveEnabled)
    {
        SaveGameData(TEXT("AutoSave"));
    }
}