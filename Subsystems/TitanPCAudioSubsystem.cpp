#include "TitanPCAudioSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Kismet/GameplayStatics.h"
#include "AudioMixerSubsystem.h"

void UTitanPCAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    InitializeAudioSystem();
    LoadAudioSettings();
    
    UE_LOG(LogTemp, Warning, TEXT("TitanPC Audio Subsystem Initialized"));
}

void UTitanPCAudioSubsystem::Deinitialize()
{
    SaveAudioSettings();
    
    if (CurrentMusicComponent && IsValid(CurrentMusicComponent))
    {
        CurrentMusicComponent->Stop();
    }
    
    if (CurrentVoiceComponent && IsValid(CurrentVoiceComponent))
    {
        CurrentVoiceComponent->Stop();
    }
    
    Super::Deinitialize();
}

void UTitanPCAudioSubsystem::InitializeAudioSystem()
{
    // Load sound classes if specified
    if (MasterSoundClass.IsValid())
    {
        LoadedMasterSoundClass = MasterSoundClass.LoadSynchronous();
    }
    
    if (MusicSoundClass.IsValid())
    {
        LoadedMusicSoundClass = MusicSoundClass.LoadSynchronous();
    }
    
    if (SFXSoundClass.IsValid())
    {
        LoadedSFXSoundClass = SFXSoundClass.LoadSynchronous();
    }
    
    if (VoiceSoundClass.IsValid())
    {
        LoadedVoiceSoundClass = VoiceSoundClass.LoadSynchronous();
    }
    
    if (UISoundClass.IsValid())
    {
        LoadedUISoundClass = UISoundClass.LoadSynchronous();
    }
    
    if (GameSoundMix.IsValid())
    {
        LoadedGameSoundMix = GameSoundMix.LoadSynchronous();
    }
}

void UTitanPCAudioSubsystem::SetMasterVolume(float Volume)
{
    MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    UpdateSoundMix();
    OnVolumeChanged.Broadcast(TEXT("Master"), MasterVolume);
}

void UTitanPCAudioSubsystem::SetMusicVolume(float Volume)
{
    MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    UpdateSoundMix();
    OnVolumeChanged.Broadcast(TEXT("Music"), MusicVolume);
}

void UTitanPCAudioSubsystem::SetSFXVolume(float Volume)
{
    SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    UpdateSoundMix();
    OnVolumeChanged.Broadcast(TEXT("SFX"), SFXVolume);
}

void UTitanPCAudioSubsystem::SetVoiceVolume(float Volume)
{
    VoiceVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    UpdateSoundMix();
    OnVolumeChanged.Broadcast(TEXT("Voice"), VoiceVolume);
}

void UTitanPCAudioSubsystem::SetUIVolume(float Volume)
{
    UIVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    UpdateSoundMix();
    OnVolumeChanged.Broadcast(TEXT("UI"), UIVolume);
}

void UTitanPCAudioSubsystem::SetMasterMute(bool bMute)
{
    bMasterMuted = bMute;
    UpdateSoundMix();
    OnMasterMuteChanged.Broadcast(bMasterMuted);
}

void UTitanPCAudioSubsystem::PlayMusic(USoundBase* MusicTrack, bool bLoop, float FadeInTime)
{
    if (!MusicTrack) return;

    UWorld* World = GetWorld();
    if (!World) return;

    // Stop current music if playing
    if (CurrentMusicComponent && IsValid(CurrentMusicComponent))
    {
        CurrentMusicComponent->FadeOut(FadeInTime, 0.0f);
    }

    // Create new music component
    CurrentMusicComponent = UGameplayStatics::SpawnSound2D(World, MusicTrack, MusicVolume * MasterVolume, 1.0f, 0.0f, nullptr, true, false);
    
    if (CurrentMusicComponent)
    {
        CurrentMusicComponent->bIsUISound = false;
        CurrentMusicComponent->bAllowSpatialization = false;
        
        if (LoadedMusicSoundClass)
        {
            CurrentMusicComponent->SoundClassOverride = LoadedMusicSoundClass;
        }
        
        if (FadeInTime > 0.0f)
        {
            CurrentMusicComponent->FadeIn(FadeInTime, MusicVolume * MasterVolume);
        }
        else
        {
            CurrentMusicComponent->Play();
        }
    }
}

void UTitanPCAudioSubsystem::StopMusic(float FadeOutTime)
{
    if (CurrentMusicComponent && IsValid(CurrentMusicComponent))
    {
        if (FadeOutTime > 0.0f)
        {
            CurrentMusicComponent->FadeOut(FadeOutTime, 0.0f);
        }
        else
        {
            CurrentMusicComponent->Stop();
        }
    }
}

void UTitanPCAudioSubsystem::PauseMusic()
{
    if (CurrentMusicComponent && IsValid(CurrentMusicComponent))
    {
        CurrentMusicComponent->SetPaused(true);
    }
}

void UTitanPCAudioSubsystem::ResumeMusic()
{
    if (CurrentMusicComponent && IsValid(CurrentMusicComponent))
    {
        CurrentMusicComponent->SetPaused(false);
    }
}

bool UTitanPCAudioSubsystem::IsMusicPlaying() const
{
    return CurrentMusicComponent && IsValid(CurrentMusicComponent) && CurrentMusicComponent->IsPlaying();
}

void UTitanPCAudioSubsystem::PlaySFX(USoundBase* SFXSound, FVector Location, float VolumeMultiplier)
{
    if (!SFXSound) return;

    UWorld* World = GetWorld();
    if (!World) return;

    float AdjustedVolume = SFXVolume * MasterVolume * VolumeMultiplier;
    
    if (Location == FVector::ZeroVector)
    {
        // Play as 2D sound
        UGameplayStatics::PlaySound2D(World, SFXSound, AdjustedVolume);
    }
    else
    {
        // Play as 3D positional sound
        UGameplayStatics::PlaySoundAtLocation(World, SFXSound, Location, AdjustedVolume);
    }
}

void UTitanPCAudioSubsystem::PlayUISFX(USoundBase* UISound, float VolumeMultiplier)
{
    if (!UISound) return;

    UWorld* World = GetWorld();
    if (!World) return;

    float AdjustedVolume = UIVolume * MasterVolume * VolumeMultiplier;
    UGameplayStatics::PlaySound2D(World, UISound, AdjustedVolume);
}

void UTitanPCAudioSubsystem::PlayVoice(USoundBase* VoiceClip, bool bInterruptCurrent)
{
    if (!VoiceClip) return;

    UWorld* World = GetWorld();
    if (!World) return;

    // Stop current voice if interrupting
    if (bInterruptCurrent && CurrentVoiceComponent && IsValid(CurrentVoiceComponent))
    {
        CurrentVoiceComponent->Stop();
    }

    // Don't play if voice is already playing and not interrupting
    if (!bInterruptCurrent && IsVoicePlaying())
    {
        return;
    }

    float AdjustedVolume = VoiceVolume * MasterVolume;
    CurrentVoiceComponent = UGameplayStatics::SpawnSound2D(World, VoiceClip, AdjustedVolume, 1.0f, 0.0f, nullptr, false, false);
    
    if (CurrentVoiceComponent && LoadedVoiceSoundClass)
    {
        CurrentVoiceComponent->SoundClassOverride = LoadedVoiceSoundClass;
    }
}

void UTitanPCAudioSubsystem::StopVoice()
{
    if (CurrentVoiceComponent && IsValid(CurrentVoiceComponent))
    {
        CurrentVoiceComponent->Stop();
    }
}

bool UTitanPCAudioSubsystem::IsVoicePlaying() const
{
    return CurrentVoiceComponent && IsValid(CurrentVoiceComponent) && CurrentVoiceComponent->IsPlaying();
}

void UTitanPCAudioSubsystem::ApplyAudioSettings()
{
    UpdateSoundMix();
    UE_LOG(LogTemp, Warning, TEXT("Audio settings applied"));
}

void UTitanPCAudioSubsystem::OptimizeAudioForPlatform()
{
    // Platform-specific audio optimizations
    FString PlatformName = FPlatformProperties::PlatformName();
    
    if (PlatformName.Contains(TEXT("Android")) || PlatformName.Contains(TEXT("iOS")))
    {
        // Mobile audio optimizations
        // Reduce audio quality for mobile to save memory/CPU
        UE_LOG(LogTemp, Warning, TEXT("Applying mobile audio optimizations"));
    }
    else
    {
        // PC audio optimizations
        UE_LOG(LogTemp, Warning, TEXT("Applying PC audio optimizations"));
    }
}

void UTitanPCAudioSubsystem::LoadAudioSettings()
{
    // Load from config
    GConfig->GetFloat(TEXT("TitanPC.Audio"), TEXT("MasterVolume"), MasterVolume, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Audio"), TEXT("MusicVolume"), MusicVolume, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Audio"), TEXT("SFXVolume"), SFXVolume, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Audio"), TEXT("VoiceVolume"), VoiceVolume, GGameUserSettingsIni);
    GConfig->GetFloat(TEXT("TitanPC.Audio"), TEXT("UIVolume"), UIVolume, GGameUserSettingsIni);
    GConfig->GetBool(TEXT("TitanPC.Audio"), TEXT("MasterMuted"), bMasterMuted, GGameUserSettingsIni);
}

void UTitanPCAudioSubsystem::SaveAudioSettings()
{
    // Save to config
    GConfig->SetFloat(TEXT("TitanPC.Audio"), TEXT("MasterVolume"), MasterVolume, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Audio"), TEXT("MusicVolume"), MusicVolume, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Audio"), TEXT("SFXVolume"), SFXVolume, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Audio"), TEXT("VoiceVolume"), VoiceVolume, GGameUserSettingsIni);
    GConfig->SetFloat(TEXT("TitanPC.Audio"), TEXT("UIVolume"), UIVolume, GGameUserSettingsIni);
    GConfig->SetBool(TEXT("TitanPC.Audio"), TEXT("MasterMuted"), bMasterMuted, GGameUserSettingsIni);
    GConfig->Flush(false, GGameUserSettingsIni);
}

void UTitanPCAudioSubsystem::UpdateSoundMix()
{
    if (!LoadedGameSoundMix) return;

    UWorld* World = GetWorld();
    if (!World) return;

    // Apply sound mix with current volume settings
    float FinalMasterVolume = bMasterMuted ? 0.0f : MasterVolume;
    
    // In a real implementation, you would set specific sound class volumes
    // For now, we'll use the audio mixer subsystem if available
    if (UAudioMixerSubsystem* AudioMixer = World->GetSubsystem<UAudioMixerSubsystem>())
    {
        // Apply the sound mix
        UGameplayStatics::PushSoundMixModifier(World, LoadedGameSoundMix);
    }
}