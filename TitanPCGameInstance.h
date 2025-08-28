
#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameplayTagContainer.h"
#include "TitanPCGameInstance.generated.h"

class UTitanPCInventorySubsystem;
class UTitanPCSaveGameSubsystem;
class UTitanPCSettingsSubsystem;
class UTitanPCAudioSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlatformChanged, FString, PlatformName);

UCLASS()
class TITANPC_API UTitanPCGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UTitanPCGameInstance();
    
    virtual void Init() override;
    virtual void Shutdown() override;

    // Performance Management
    UFUNCTION(BlueprintCallable, Category="Performance")
    void SetTargetFramerate(int32 FPS);

    UFUNCTION(BlueprintPure, Category="Performance")
    int32 GetTargetFramerate() const { return TargetFPS; }

    UFUNCTION(BlueprintCallable, Category="Performance")
    void OptimizeForPlatform();

    // Platform Detection
    UFUNCTION(BlueprintPure, Category="Platform")
    bool IsMobilePlatform() const;

    UFUNCTION(BlueprintPure, Category="Platform")
    FString GetCurrentPlatformName() const;

    UPROPERTY(BlueprintAssignable, Category="Platform")
    FOnPlatformChanged OnPlatformChanged;

    // Game State Management
    UFUNCTION(BlueprintCallable, Category="Game State")
    void SaveGame();

    UFUNCTION(BlueprintCallable, Category="Game State")
    void LoadGame();

    // Subsystem Access
    UFUNCTION(BlueprintPure, Category="Subsystems")
    UTitanPCInventorySubsystem* GetInventorySubsystem() const;

    UFUNCTION(BlueprintPure, Category="Subsystems")
    UTitanPCSaveGameSubsystem* GetSaveGameSubsystem() const;

    UFUNCTION(BlueprintPure, Category="Subsystems")
    UTitanPCSettingsSubsystem* GetSettingsSubsystem() const;

    UFUNCTION(BlueprintPure, Category="Subsystems")
    UTitanPCAudioSubsystem* GetAudioSubsystem() const;

protected:
    virtual void OnStart() override;

private:
    void InitializePlatformSettings();
    void ConfigureQualitySettings();
    void SetupInputModeForPlatform();

    UPROPERTY(EditDefaultsOnly, Category="Performance")
    int32 TargetFPS = 120;

    UPROPERTY(EditDefaultsOnly, Category="Performance")
    int32 MobileTargetFPS = 60;

    UPROPERTY(EditDefaultsOnly, Category="Quality")
    int32 PCQualityLevel = 4; // Epic

    UPROPERTY(EditDefaultsOnly, Category="Quality")
    int32 MobileQualityLevel = 1; // Low-Medium

    FString CurrentPlatform;
    bool bHasInitialized = false;
};
