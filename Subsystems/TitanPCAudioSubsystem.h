#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "TitanPCAudioSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVolumeChanged, FString, SoundCategory, float, NewVolume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMasterMuteChanged, bool, bIsMuted);

UCLASS()
class TITANPC_API UTitanPCAudioSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Volume Control
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMusicVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetVoiceVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetUIVolume(float Volume);

    UFUNCTION(BlueprintPure, Category = "Audio")
    float GetMasterVolume() const { return MasterVolume; }

    UFUNCTION(BlueprintPure, Category = "Audio")
    float GetMusicVolume() const { return MusicVolume; }

    UFUNCTION(BlueprintPure, Category = "Audio")
    float GetSFXVolume() const { return SFXVolume; }

    UFUNCTION(BlueprintPure, Category = "Audio")
    float GetVoiceVolume() const { return VoiceVolume; }

    UFUNCTION(BlueprintPure, Category = "Audio")
    float GetUIVolume() const { return UIVolume; }

    // Mute Control
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMasterMute(bool bMute);

    UFUNCTION(BlueprintPure, Category = "Audio")
    bool IsMasterMuted() const { return bMasterMuted; }

    // Music Control
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayMusic(USoundBase* MusicTrack, bool bLoop = true, float FadeInTime = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopMusic(float FadeOutTime = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PauseMusic();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void ResumeMusic();

    UFUNCTION(BlueprintPure, Category = "Audio")
    bool IsMusicPlaying() const;

    // SFX Control
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySFX(USoundBase* SFXSound, FVector Location = FVector::ZeroVector, float VolumeMultiplier = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayUISFX(USoundBase* UISound, float VolumeMultiplier = 1.0f);

    // Voice Control
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayVoice(USoundBase* VoiceClip, bool bInterruptCurrent = true);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopVoice();

    UFUNCTION(BlueprintPure, Category = "Audio")
    bool IsVoicePlaying() const;

    // Audio System Management
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void ApplyAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void OptimizeAudioForPlatform();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Audio")
    FOnVolumeChanged OnVolumeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Audio")
    FOnMasterMuteChanged OnMasterMuteChanged;

protected:
    void InitializeAudioSystem();
    void LoadAudioSettings();
    void SaveAudioSettings();
    void UpdateSoundMix();

private:
    // Volume settings
    float MasterVolume = 1.0f;
    float MusicVolume = 0.8f;
    float SFXVolume = 1.0f;
    float VoiceVolume = 1.0f;
    float UIVolume = 0.7f;
    bool bMasterMuted = false;

    // Audio components
    UPROPERTY()
    class UAudioComponent* CurrentMusicComponent;

    UPROPERTY()
    class UAudioComponent* CurrentVoiceComponent;

    // Sound classes for volume control
    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TSoftObjectPtr<USoundClass> MasterSoundClass;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TSoftObjectPtr<USoundClass> MusicSoundClass;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TSoftObjectPtr<USoundClass> SFXSoundClass;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TSoftObjectPtr<USoundClass> VoiceSoundClass;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TSoftObjectPtr<USoundClass> UISoundClass;

    // Sound mix for dynamic volume control
    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TSoftObjectPtr<USoundMix> GameSoundMix;

    // Loaded sound classes
    UPROPERTY()
    USoundClass* LoadedMasterSoundClass;

    UPROPERTY()
    USoundClass* LoadedMusicSoundClass;

    UPROPERTY()
    USoundClass* LoadedSFXSoundClass;

    UPROPERTY()
    USoundClass* LoadedVoiceSoundClass;

    UPROPERTY()
    USoundClass* LoadedUISoundClass;

    UPROPERTY()
    USoundMix* LoadedGameSoundMix;
};