#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TitanPCSettingsSubsystem.generated.h"

USTRUCT(BlueprintType)
struct TITANPC_API FTitanPCGraphicsSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    int32 ResolutionX = 1920;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    int32 ResolutionY = 1080;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    int32 WindowMode = 0; // 0=Fullscreen, 1=Windowed Fullscreen, 2=Windowed

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    int32 OverallQuality = 3; // 0=Low, 1=Medium, 2=High, 3=Epic

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    bool bVSyncEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    int32 FrameRateLimit = 120;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    float ResolutionScale = 100.0f;
};

USTRUCT(BlueprintType)
struct TITANPC_API FTitanPCAudioSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MasterVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MusicVolume = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float SFXVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float VoiceVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    bool bMasterMuted = false;
};

USTRUCT(BlueprintType)
struct TITANPC_API FTitanPCInputSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float MouseSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    float TouchSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool bInvertMouseY = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool bInvertTouchY = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool bShowTouchControls = true; // For mobile
};

USTRUCT(BlueprintType)
struct TITANPC_API FTitanPCGameplaySettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    int32 Difficulty = 1; // 0=Easy, 1=Normal, 2=Hard, 3=Expert

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bSubtitlesEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    float SubtitleSize = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bAutoSaveEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    float AutoSaveInterval = 300.0f; // 5 minutes
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingsChanged, FString, SettingCategory);

UCLASS()
class TITANPC_API UTitanPCSettingsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Settings Management
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void LoadSettings();

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SaveSettings();

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ResetToDefaults();

    // Graphics Settings
    UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
    void SetGraphicsSettings(const FTitanPCGraphicsSettings& NewSettings);

    UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
    FTitanPCGraphicsSettings GetGraphicsSettings() const { return GraphicsSettings; }

    UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
    void ApplyGraphicsSettings();

    // Audio Settings
    UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
    void SetAudioSettings(const FTitanPCAudioSettings& NewSettings);

    UFUNCTION(BlueprintPure, Category = "Settings|Audio")
    FTitanPCAudioSettings GetAudioSettings() const { return AudioSettings; }

    UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
    void ApplyAudioSettings();

    // Input Settings
    UFUNCTION(BlueprintCallable, Category = "Settings|Input")
    void SetInputSettings(const FTitanPCInputSettings& NewSettings);

    UFUNCTION(BlueprintPure, Category = "Settings|Input")
    FTitanPCInputSettings GetInputSettings() const { return InputSettings; }

    UFUNCTION(BlueprintCallable, Category = "Settings|Input")
    void ApplyInputSettings();

    // Gameplay Settings
    UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
    void SetGameplaySettings(const FTitanPCGameplaySettings& NewSettings);

    UFUNCTION(BlueprintPure, Category = "Settings|Gameplay")
    FTitanPCGameplaySettings GetGameplaySettings() const { return GameplaySettings; }

    UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
    void ApplyGameplaySettings();

    // Platform-specific
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void OptimizeSettingsForPlatform();

    UFUNCTION(BlueprintPure, Category = "Settings")
    bool IsMobilePlatform() const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Settings")
    FOnSettingsChanged OnSettingsChanged;

private:
    void LoadDefaultSettings();
    void LoadPlatformSpecificDefaults();
    void SaveToConfig();
    void LoadFromConfig();

    UPROPERTY()
    FTitanPCGraphicsSettings GraphicsSettings;

    UPROPERTY()
    FTitanPCAudioSettings AudioSettings;

    UPROPERTY()
    FTitanPCInputSettings InputSettings;

    UPROPERTY()
    FTitanPCGameplaySettings GameplaySettings;

    FString SettingsFileName = TEXT("TitanPCSettings");
    bool bSettingsLoaded = false;
};