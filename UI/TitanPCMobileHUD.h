#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TitanPCMobileHUD.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMobileButtonPressed);

UCLASS()
class TITANPC_API UTitanPCMobileHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // Touch Control Events
    UPROPERTY(BlueprintAssignable, Category = "Mobile Controls")
    FOnMobileButtonPressed OnFirePressed;

    UPROPERTY(BlueprintAssignable, Category = "Mobile Controls")
    FOnMobileButtonPressed OnJumpPressed;

    UPROPERTY(BlueprintAssignable, Category = "Mobile Controls")
    FOnMobileButtonPressed OnReloadPressed;

    UPROPERTY(BlueprintAssignable, Category = "Mobile Controls")
    FOnMobileButtonPressed OnSprintPressed;

    UPROPERTY(BlueprintAssignable, Category = "Mobile Controls")
    FOnMobileButtonPressed OnAimPressed;

    UPROPERTY(BlueprintAssignable, Category = "Mobile Controls")
    FOnMobileButtonPressed OnMeleePressed;

    // Update functions
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateStaminaBar(float CurrentStamina, float MaxStamina);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAmmoDisplay(int32 CurrentAmmo, int32 ReserveAmmo);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetTouchControlsVisible(bool bVisible);

    UFUNCTION(BlueprintPure, Category = "HUD")
    bool ShouldShowTouchControls() const;

protected:
    // Health and Status UI
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* StaminaBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* ShieldBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* StaminaText;

    // Ammo and Weapon UI
    UPROPERTY(meta = (BindWidget))
    UTextBlock* AmmoText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ReserveAmmoText;

    UPROPERTY(meta = (BindWidget))
    UImage* WeaponIcon;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* ReloadProgressBar;

    // Mobile Touch Controls
    UPROPERTY(meta = (BindWidget))
    UButton* FireButton;

    UPROPERTY(meta = (BindWidget))
    UButton* JumpButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ReloadButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SprintButton;

    UPROPERTY(meta = (BindWidget))
    UButton* AimButton;

    UPROPERTY(meta = (BindWidget))
    UButton* MeleeButton;

    // Virtual Joysticks
    UPROPERTY(meta = (BindWidget))
    class UTitanPCVirtualJoystick* MovementJoystick;

    UPROPERTY(meta = (BindWidget))
    class UTitanPCVirtualJoystick* LookJoystick;

    // Crosshair
    UPROPERTY(meta = (BindWidget))
    UImage* Crosshair;

    // Minimap
    UPROPERTY(meta = (BindWidget))
    UImage* Minimap;

    // Character info
    UPROPERTY(meta = (BindWidget))
    UTextBlock* LevelText;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* ExperienceBar;

private:
    // Button event handlers
    UFUNCTION()
    void OnFireButtonPressed();

    UFUNCTION()
    void OnJumpButtonPressed();

    UFUNCTION()
    void OnReloadButtonPressed();

    UFUNCTION()
    void OnSprintButtonPressed();

    UFUNCTION()
    void OnAimButtonPressed();

    UFUNCTION()
    void OnMeleeButtonPressed();

    void BindButtonEvents();
    void UpdatePlatformSpecificUI();

    bool bTouchControlsVisible = true;
};