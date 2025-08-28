
#include "TitanPCGameInstance.h"
#include "Engine/Engine.h"
#include "HAL/ConsoleManager.h"
#include "Engine/World.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Subsystems/TitanPCInventorySubsystem.h"
#include "Subsystems/TitanPCSaveGameSubsystem.h"
#include "Subsystems/TitanPCSettingsSubsystem.h"
#include "Subsystems/TitanPCAudioSubsystem.h"

UTitanPCGameInstance::UTitanPCGameInstance()
{
    CurrentPlatform = FPlatformProperties::PlatformName();
}

void UTitanPCGameInstance::Init()
{
    Super::Init();
    
    InitializePlatformSettings();
    ConfigureQualitySettings();
    OptimizeForPlatform();
    
    bHasInitialized = true;
    OnPlatformChanged.Broadcast(CurrentPlatform);
    
    UE_LOG(LogTemp, Warning, TEXT("TitanPC Game Instance Initialized for platform: %s"), *CurrentPlatform);
}

void UTitanPCGameInstance::Shutdown()
{
    // Clean shutdown of subsystems
    if (UTitanPCSaveGameSubsystem* SaveSystem = GetSaveGameSubsystem())
    {
        SaveGame(); // Auto-save on shutdown
    }
    
    Super::Shutdown();
}

void UTitanPCGameInstance::OnStart()
{
    Super::OnStart();
    SetupInputModeForPlatform();
}

void UTitanPCGameInstance::SetTargetFramerate(int32 FPS)
{
    int32 ClampedFPS = IsMobilePlatform() ? 
        FMath::Clamp(FPS, 30, MobileTargetFPS) : 
        FMath::Clamp(FPS, 30, 240);
    
    TargetFPS = ClampedFPS;
    
    IConsoleVariable* MaxFPSCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
    if (MaxFPSCVar)
    {
        MaxFPSCVar->Set(TargetFPS, ECVF_SetByGameSetting);
    }
    
    // Also set VSync appropriately
    if (UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings())
    {
        Settings->SetVSyncEnabled(IsMobilePlatform() ? true : false);
        Settings->ApplySettings(false);
    }
}

void UTitanPCGameInstance::OptimizeForPlatform()
{
    if (IsMobilePlatform())
    {
        // Mobile optimizations
        SetTargetFramerate(MobileTargetFPS);
        
        // Reduce quality settings for mobile
        IConsoleVariable* ShadowQualityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.ShadowQuality"));
        if (ShadowQualityCVar) ShadowQualityCVar->Set(1, ECVF_SetByGameSetting);
        
        IConsoleVariable* PostProcessQualityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.PostProcessQuality"));
        if (PostProcessQualityCVar) PostProcessQualityCVar->Set(1, ECVF_SetByGameSetting);
        
        IConsoleVariable* TextureQualityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.TextureQuality"));
        if (TextureQualityCVar) TextureQualityCVar->Set(2, ECVF_SetByGameSetting);
        
        IConsoleVariable* EffectsQualityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.EffectsQuality"));
        if (EffectsQualityCVar) EffectsQualityCVar->Set(1, ECVF_SetByGameSetting);
        
        // Disable expensive features on mobile
        IConsoleVariable* LumenCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.GlobalIllumination"));
        if (LumenCVar) LumenCVar->Set(0, ECVF_SetByGameSetting);
        
        IConsoleVariable* NaniteCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Nanite"));
        if (NaniteCVar) NaniteCVar->Set(0, ECVF_SetByGameSetting);
    }
    else
    {
        // PC optimizations
        SetTargetFramerate(TargetFPS);
        
        // Enable high-quality features on PC
        IConsoleVariable* ShadowQualityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.ShadowQuality"));
        if (ShadowQualityCVar) ShadowQualityCVar->Set(PCQualityLevel, ECVF_SetByGameSetting);
        
        IConsoleVariable* PostProcessQualityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.PostProcessQuality"));
        if (PostProcessQualityCVar) PostProcessQualityCVar->Set(PCQualityLevel, ECVF_SetByGameSetting);
        
        IConsoleVariable* TextureQualityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.TextureQuality"));
        if (TextureQualityCVar) TextureQualityCVar->Set(PCQualityLevel, ECVF_SetByGameSetting);
        
        IConsoleVariable* EffectsQualityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("sg.EffectsQuality"));
        if (EffectsQualityCVar) EffectsQualityCVar->Set(PCQualityLevel, ECVF_SetByGameSetting);
    }
}

bool UTitanPCGameInstance::IsMobilePlatform() const
{
    return CurrentPlatform.Contains(TEXT("Android")) || CurrentPlatform.Contains(TEXT("iOS"));
}

FString UTitanPCGameInstance::GetCurrentPlatformName() const
{
    return CurrentPlatform;
}

void UTitanPCGameInstance::SaveGame()
{
    if (UTitanPCSaveGameSubsystem* SaveSystem = GetSaveGameSubsystem())
    {
        SaveSystem->SaveGameData();
    }
}

void UTitanPCGameInstance::LoadGame()
{
    if (UTitanPCSaveGameSubsystem* SaveSystem = GetSaveGameSubsystem())
    {
        SaveSystem->LoadGameData();
    }
}

UTitanPCInventorySubsystem* UTitanPCGameInstance::GetInventorySubsystem() const
{
    return GetSubsystem<UTitanPCInventorySubsystem>();
}

UTitanPCSaveGameSubsystem* UTitanPCGameInstance::GetSaveGameSubsystem() const
{
    return GetSubsystem<UTitanPCSaveGameSubsystem>();
}

UTitanPCSettingsSubsystem* UTitanPCGameInstance::GetSettingsSubsystem() const
{
    return GetSubsystem<UTitanPCSettingsSubsystem>();
}

UTitanPCAudioSubsystem* UTitanPCGameInstance::GetAudioSubsystem() const
{
    return GetSubsystem<UTitanPCAudioSubsystem>();
}

void UTitanPCGameInstance::InitializePlatformSettings()
{
    CurrentPlatform = FPlatformProperties::PlatformName();
    
    // Platform-specific initialization
    if (IsMobilePlatform())
    {
        // Mobile-specific settings
        GConfig->SetBool(TEXT("/Script/Engine.RendererSettings"), TEXT("r.MobileHDR"), true, GEngineIni);
        GConfig->SetBool(TEXT("/Script/Engine.RendererSettings"), TEXT("r.Mobile.EnableStaticAndCSMShadowReceivers"), true, GEngineIni);
        GConfig->SetInt(TEXT("/Script/Engine.RendererSettings"), TEXT("r.DefaultFeature.AutoExposure"), 1, GEngineIni);
    }
    else
    {
        // PC-specific settings
        GConfig->SetBool(TEXT("/Script/Engine.RendererSettings"), TEXT("r.AllowGlobalClipPlane"), true, GEngineIni);
        GConfig->SetBool(TEXT("/Script/Engine.RendererSettings"), TEXT("r.GBufferFormat"), true, GEngineIni);
    }
}

void UTitanPCGameInstance::ConfigureQualitySettings()
{
    UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
    if (!Settings) return;
    
    if (IsMobilePlatform())
    {
        Settings->SetOverallScalabilityLevel(MobileQualityLevel);
        Settings->SetResolutionScaleValueEx(75.0f); // Reduce resolution scale for mobile
    }
    else
    {
        Settings->SetOverallScalabilityLevel(PCQualityLevel);
        Settings->SetResolutionScaleValueEx(100.0f);
    }
    
    Settings->ApplySettings(false);
}

void UTitanPCGameInstance::SetupInputModeForPlatform()
{
    if (APlayerController* PC = GetFirstLocalPlayerController())
    {
        if (IsMobilePlatform())
        {
            // Touch-friendly input mode
            FInputModeGameAndUI InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            InputMode.SetHideCursorDuringCapture(false);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
        else
        {
            // Traditional PC input mode
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
        }
    }
}
