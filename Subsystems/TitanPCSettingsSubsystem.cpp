#include "TitanPCSettingsSubsystem.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "HAL/PlatformApplicationMisc.h"
#include "HAL/ConsoleManager.h"
#include "Audio/AudioMixerDevice.h"
#include "AudioMixerSubsystem.h"

void UTitanPCSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    LoadDefaultSettings();
    LoadSettings();
    
    UE_LOG(LogTemp, Warning, TEXT("TitanPC Settings Subsystem Initialized"));
}

void UTitanPCSettingsSubsystem::Deinitialize()
{
    SaveSettings();
    Super::Deinitialize();
}

void UTitanPCSettingsSubsystem::LoadSettings()
{
    if (!bSettingsLoaded)
    {
        LoadFromConfig();
        bSettingsLoaded = true;
    }
    
    // Apply loaded settings
    ApplyGraphicsSettings();
    ApplyAudioSettings();
    ApplyInputSettings();
    ApplyGameplaySettings();
}

void UTitanPCSettingsSubsystem::SaveSettings()
{
    SaveToConfig();
    UE_LOG(LogTemp, Warning, TEXT("Settings saved"));
}

void UTitanPCSettingsSubsystem::ResetToDefaults()
{
    LoadDefaultSettings();
    LoadPlatformSpecificDefaults();
    
    ApplyGraphicsSettings();
    ApplyAudioSettings();
    ApplyInputSettings();
    ApplyGameplaySettings();
    
    OnSettingsChanged.Broadcast(TEXT("All"));
}

void UTitanPCSettingsSubsystem::SetGraphicsSettings(const FTitanPCGraphicsSettings& NewSettings)
{
    GraphicsSettings = NewSettings;
    ApplyGraphicsSettings();
    OnSettingsChanged.Broadcast(TEXT("Graphics"));
}

void UTitanPCSettingsSubsystem::ApplyGraphicsSettings()
{
    UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
    if (!Settings) return;
    
    // Resolution and window mode
    Settings->SetScreenResolution(FIntPoint(GraphicsSettings.ResolutionX, GraphicsSettings.ResolutionY));
    Settings->SetFullscreenMode(static_cast<EWindowMode::Type>(GraphicsSettings.WindowMode));
    
    // Quality settings
    Settings->SetOverallScalabilityLevel(GraphicsSettings.OverallQuality);
    Settings->SetResolutionScaleValueEx(GraphicsSettings.ResolutionScale);
    
    // VSync
    Settings->SetVSyncEnabled(GraphicsSettings.bVSyncEnabled);
    
    // Frame rate limit
    IConsoleVariable* MaxFPSCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
    if (MaxFPSCVar)
    {
        MaxFPSCVar->Set(GraphicsSettings.FrameRateLimit, ECVF_SetByGameSetting);
    }
    
    Settings->ApplySettings(false);
    UE_LOG(LogTemp, Warning, TEXT("Graphics settings applied"));
}

void UTitanPCSettingsSubsystem::SetAudioSettings(const FTitanPCAudioSettings& NewSettings)
{
    AudioSettings = NewSettings;
    ApplyAudioSettings();
    OnSettingsChanged.Broadcast(TEXT("Audio"));
}

void UTitanPCSettingsSubsystem::ApplyAudioSettings()
{
    if (UWorld* World = GetWorld())
    {
        if (UAudioMixerSubsystem* AudioMixer = World->GetSubsystem<UAudioMixerSubsystem>())
        {
            // Apply volume settings
            // Note: In a real implementation, you'd use Sound Classes and Sound Mixes
            UE_LOG(LogTemp, Warning, TEXT("Audio settings applied - Master: %f, Music: %f, SFX: %f"), 
                AudioSettings.MasterVolume, AudioSettings.MusicVolume, AudioSettings.SFXVolume);
        }
    }
}

void UTitanPCSettingsSubsystem::SetInputSettings(const FTitanPCInputSettings& NewSettings)
{
    InputSettings = NewSettings;
    ApplyInputSettings();
    OnSettingsChanged.Broadcast(TEXT("Input"));
}

void UTitanPCSettingsSubsystem::ApplyInputSettings()
{
    // Apply mouse sensitivity
    IConsoleVariable* MouseSensCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("input.MouseSensitivity"));
    if (MouseSensCVar)
    {
        MouseSensCVar->Set(InputSettings.MouseSensitivity, ECVF_SetByGameSetting);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Input settings applied - Mouse Sensitivity: %f"), InputSettings.MouseSensitivity);
}

void UTitanPCSettingsSubsystem::SetGameplaySettings(const FTitanPCGameplaySettings& NewSettings)
{
    GameplaySettings = NewSettings;
    ApplyGameplaySettings();
    OnSettingsChanged.Broadcast(TEXT("Gameplay"));
}

void UTitanPCSettingsSubsystem::ApplyGameplaySettings()
{
    // Auto-save configuration would be handled by the save system
    UE_LOG(LogTemp, Warning, TEXT("Gameplay settings applied - Difficulty: %d, Auto-save: %s"), 
        GameplaySettings.Difficulty, GameplaySettings.bAutoSaveEnabled ? TEXT("True") : TEXT("False"));
}

void UTitanPCSettingsSubsystem::OptimizeSettingsForPlatform()
{
    LoadPlatformSpecificDefaults();
    
    ApplyGraphicsSettings();
    ApplyAudioSettings();
    ApplyInputSettings();
    ApplyGameplaySettings();
    
    OnSettingsChanged.Broadcast(TEXT("Platform"));
}

bool UTitanPCSettingsSubsystem::IsMobilePlatform() const
{
    FString PlatformName = FPlatformProperties::PlatformName();
    return PlatformName.Contains(TEXT("Android")) || PlatformName.Contains(TEXT("iOS"));
}

void UTitanPCSettingsSubsystem::LoadDefaultSettings()
{
    // Graphics defaults
    GraphicsSettings = FTitanPCGraphicsSettings();
    
    // Audio defaults
    AudioSettings = FTitanPCAudioSettings();
    
    // Input defaults
    InputSettings = FTitanPCInputSettings();
    
    // Gameplay defaults
    GameplaySettings = FTitanPCGameplaySettings();
    
    LoadPlatformSpecificDefaults();
}

void UTitanPCSettingsSubsystem::LoadPlatformSpecificDefaults()
{
    if (IsMobilePlatform())
    {
        // Mobile-specific defaults
        GraphicsSettings.ResolutionX = 1920;
        GraphicsSettings.ResolutionY = 1080;
        GraphicsSettings.WindowMode = 0; // Fullscreen
        GraphicsSettings.OverallQuality = 1; // Medium
        GraphicsSettings.bVSyncEnabled = true;
        GraphicsSettings.FrameRateLimit = 60;
        GraphicsSettings.ResolutionScale = 75.0f; // Reduce for mobile
        
        InputSettings.bShowTouchControls = true;
        InputSettings.TouchSensitivity = 1.0f;
        
        GameplaySettings.AutoSaveInterval = 180.0f; // More frequent on mobile
    }
    else
    {
        // PC-specific defaults
        GraphicsSettings.ResolutionX = 1920;
        GraphicsSettings.ResolutionY = 1080;
        GraphicsSettings.WindowMode = 0; // Fullscreen
        GraphicsSettings.OverallQuality = 3; // Epic
        GraphicsSettings.bVSyncEnabled = false;
        GraphicsSettings.FrameRateLimit = 120;
        GraphicsSettings.ResolutionScale = 100.0f;
        
        InputSettings.bShowTouchControls = false;
        InputSettings.MouseSensitivity = 1.0f;
        
        GameplaySettings.AutoSaveInterval = 300.0f; // 5 minutes
    }
}

void UTitanPCSettingsSubsystem::SaveToConfig()
{
    // Graphics
    GConfig->SetInt(TEXT("TitanPC.Graphics"), TEXT("ResolutionX"), GraphicsSettings.ResolutionX, GGameUserSettingsIni);
    GConfig->SetInt(TEXT("TitanPC.Graphics"), TEXT("ResolutionY"), GraphicsSettings.ResolutionY, GGameUserSettingsIni);
    GConfig->SetInt(TEXT("TitanPC.Graphics"), TEXT("WindowMode"), GraphicsSettings.WindowMode, GGameUserSettingsIni);
    GConfig->SetInt(TEXT("TitanPC.Graphics"), TEXT("OverallQuality"), GraphicsSettings.OverallQuality, GGameUserSettingsIni);
    GConfig->SetBool(TEXT("TitanPC.Graphics"), TEXT("VSyncEnabled"), GraphicsSettings.bVSyncEnabled, GGameUserSettingsIni);
    GConfig->SetInt(TEXT("TitanPC.Graphics"), TEXT("FrameRateLimit"), GraphicsSettings.FrameRateLimit, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Graphics"), TEXT("ResolutionScale"), GraphicsSettings.ResolutionScale, GGameUserSettingsIni);
    
    // Audio
    GConfig->SetFloat(TEXT("TitanPC.Audio"), TEXT("MasterVolume"), AudioSettings.MasterVolume, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Audio"), TEXT("MusicVolume"), AudioSettings.MusicVolume, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Audio"), TEXT("SFXVolume"), AudioSettings.SFXVolume, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Audio"), TEXT("VoiceVolume"), AudioSettings.VoiceVolume, GGameUserSettingsIni);
    GConfig->SetBool(TEXT("TitanPC.Audio"), TEXT("MasterMuted"), AudioSettings.bMasterMuted, GGameUserSettingsIni);
    
    // Input
    GConfig->SetFloat(TEXT("TitanPC.Input"), TEXT("MouseSensitivity"), InputSettings.MouseSensitivity, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Input"), TEXT("TouchSensitivity"), InputSettings.TouchSensitivity, GGameUserSettingsIni);
    GConfig->SetBool(TEXT("TitanPC.Input"), TEXT("InvertMouseY"), InputSettings.bInvertMouseY, GGameUserSettingsIni);
    GConfig->SetBool(TEXT("TitanPC.Input"), TEXT("InvertTouchY"), InputSettings.bInvertTouchY, GGameUserSettingsIni);
    GConfig->SetBool(TEXT("TitanPC.Input"), TEXT("ShowTouchControls"), InputSettings.bShowTouchControls, GGameUserSettingsIni);
    
    // Gameplay
    GConfig->SetInt(TEXT("TitanPC.Gameplay"), TEXT("Difficulty"), GameplaySettings.Difficulty, GGameUserSettingsIni);
    GConfig->SetBool(TEXT("TitanPC.Gameplay"), TEXT("SubtitlesEnabled"), GameplaySettings.bSubtitlesEnabled, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Gameplay"), TEXT("SubtitleSize"), GameplaySettings.SubtitleSize, GGameUserSettingsIni);
    GConfig->SetBool(TEXT("TitanPC.Gameplay"), TEXT("AutoSaveEnabled"), GameplaySettings.bAutoSaveEnabled, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Gameplay"), TEXT("AutoSaveInterval"), GameplaySettings.AutoSaveInterval, GGameUserSettingsIni);
    
    GConfig->Flush(false, GGameUserSettingsIni);
}

void UTitanPCSettingsSubsystem::LoadFromConfig()
{
    // Graphics
    GConfig->GetInt(TEXT("TitanPC.Graphics"), TEXT("ResolutionX"), GraphicsSettings.ResolutionX, GGameUserSettingsIni);
    GConfig->GetInt(TEXT("TitanPC.Graphics"), TEXT("ResolutionY"), GraphicsSettings.ResolutionY, GGameUserSettingsIni);
    GConfig->GetInt(TEXT("TitanPC.Graphics"), TEXT("WindowMode"), GraphicsSettings.WindowMode, GGameUserSettingsIni);
    GConfig->GetInt(TEXT("TitanPC.Graphics"), TEXT("OverallQuality"), GraphicsSettings.OverallQuality, GGameUserSettingsIni);
    GConfig->GetBool(TEXT("TitanPC.Graphics"), TEXT("VSyncEnabled"), GraphicsSettings.bVSyncEnabled, GGameUserSettingsIni);
    GConfig->GetInt(TEXT("TitanPC.Graphics"), TEXT("FrameRateLimit"), GraphicsSettings.FrameRateLimit, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Graphics"), TEXT("ResolutionScale"), GraphicsSettings.ResolutionScale, GGameUserSettingsIni);
    
    // Audio
    GConfig->GetFloat(TEXT("TitanPC.Audio"), TEXT("MasterVolume"), AudioSettings.MasterVolume, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Audio"), TEXT("MusicVolume"), AudioSettings.MusicVolume, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Audio"), TEXT("SFXVolume"), AudioSettings.SFXVolume, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Audio"), TEXT("VoiceVolume"), AudioSettings.VoiceVolume, GGameUserSettingsIni);
    GConfig->GetBool(TEXT("TitanPC.Audio"), TEXT("MasterMuted"), AudioSettings.bMasterMuted, GGameUserSettingsIni);
    
    // Input
    GConfig->GetFloat(TEXT("TitanPC.Input"), TEXT("MouseSensitivity"), InputSettings.MouseSensitivity, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Input"), TEXT("TouchSensitivity"), InputSettings.TouchSensitivity, GGameUserSettingsIni);
    GConfig->GetBool(TEXT("TitanPC.Input"), TEXT("InvertMouseY"), InputSettings.bInvertMouseY, GGameUserSettingsIni);
    GConfig->GetBool(TEXT("TitanPC.Input"), TEXT("InvertTouchY"), InputSettings.bInvertTouchY, GGameUserSettingsIni);
    GConfig->GetBool(TEXT("TitanPC.Input"), TEXT("ShowTouchControls"), InputSettings.bShowTouchControls, GGameUserSettingsIni);
    
    // Gameplay
    GConfig->GetInt(TEXT("TitanPC.Gameplay"), TEXT("Difficulty"), GameplaySettings.Difficulty, GGameUserSettingsIni);
    GConfig->GetBool(TEXT("TitanPC.Gameplay"), TEXT("SubtitlesEnabled"), GameplaySettings.bSubtitlesEnabled, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Gameplay"), TEXT("SubtitleSize"), GameplaySettings.SubtitleSize, GGameUserSettingsIni);
    GConfig->GetBool(TEXT("TitanPC.Gameplay"), TEXT("AutoSaveEnabled"), GameplaySettings.bAutoSaveEnabled, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Gameplay"), TEXT("AutoSaveInterval"), GameplaySettings.AutoSaveInterval, GGameUserSettingsIni);
}