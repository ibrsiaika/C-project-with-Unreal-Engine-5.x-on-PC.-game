#include "TitanPCMobileHUD.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Engine.h"
#include "HAL/PlatformApplicationMisc.h"
#include "TitanPCVirtualJoystick.h"

void UTitanPCMobileHUD::NativeConstruct()
{
    Super::NativeConstruct();
    
    BindButtonEvents();
    UpdatePlatformSpecificUI();
    
    // Initialize UI elements
    if (ReloadProgressBar)
    {
        ReloadProgressBar->SetVisibility(ESlateVisibility::Hidden);
    }
    
    // Set initial crosshair
    if (Crosshair)
    {
        Crosshair->SetVisibility(ESlateVisibility::Visible);
    }
}

void UTitanPCMobileHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    // Update platform-specific visibility
    UpdatePlatformSpecificUI();
}

void UTitanPCMobileHUD::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    if (HealthBar && MaxHealth > 0.0f)
    {
        float HealthPercentage = CurrentHealth / MaxHealth;
        HealthBar->SetPercent(HealthPercentage);
        
        // Color code the health bar
        FLinearColor HealthColor = FLinearColor::Green;
        if (HealthPercentage < 0.3f)
        {
            HealthColor = FLinearColor::Red;
        }
        else if (HealthPercentage < 0.6f)
        {
            HealthColor = FLinearColor::Yellow;
        }
        
        HealthBar->SetFillColorAndOpacity(HealthColor);
    }
    
    if (HealthText)
    {
        FText HealthDisplayText = FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentHealth, MaxHealth));
        HealthText->SetText(HealthDisplayText);
    }
}

void UTitanPCMobileHUD::UpdateStaminaBar(float CurrentStamina, float MaxStamina)
{
    if (StaminaBar && MaxStamina > 0.0f)
    {
        float StaminaPercentage = CurrentStamina / MaxStamina;
        StaminaBar->SetPercent(StaminaPercentage);
        
        // Color code the stamina bar
        FLinearColor StaminaColor = FLinearColor::Blue;
        if (StaminaPercentage < 0.2f)
        {
            StaminaColor = FLinearColor(1.0f, 0.5f, 0.0f); // Orange
        }
        
        StaminaBar->SetFillColorAndOpacity(StaminaColor);
    }
    
    if (StaminaText)
    {
        FText StaminaDisplayText = FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentStamina, MaxStamina));
        StaminaText->SetText(StaminaDisplayText);
    }
}

void UTitanPCMobileHUD::UpdateAmmoDisplay(int32 CurrentAmmo, int32 ReserveAmmo)
{
    if (AmmoText)
    {
        FText AmmoDisplayText = FText::FromString(FString::Printf(TEXT("%d"), CurrentAmmo));
        AmmoText->SetText(AmmoDisplayText);
        
        // Color code ammo based on remaining
        FLinearColor AmmoColor = FLinearColor::White;
        if (CurrentAmmo == 0)
        {
            AmmoColor = FLinearColor::Red;
        }
        else if (CurrentAmmo <= 5)
        {
            AmmoColor = FLinearColor::Yellow;
        }
        
        AmmoText->SetColorAndOpacity(FSlateColor(AmmoColor));
    }
    
    if (ReserveAmmoText)
    {
        FText ReserveDisplayText = FText::FromString(FString::Printf(TEXT("%d"), ReserveAmmo));
        ReserveAmmoText->SetText(ReserveDisplayText);
    }
}

void UTitanPCMobileHUD::SetTouchControlsVisible(bool bVisible)
{
    bTouchControlsVisible = bVisible;
    
    ESlateVisibility Visibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
    
    // Hide/Show touch control buttons
    if (FireButton) FireButton->SetVisibility(Visibility);
    if (JumpButton) JumpButton->SetVisibility(Visibility);
    if (ReloadButton) ReloadButton->SetVisibility(Visibility);
    if (SprintButton) SprintButton->SetVisibility(Visibility);
    if (AimButton) AimButton->SetVisibility(Visibility);
    if (MeleeButton) MeleeButton->SetVisibility(Visibility);
    
    // Hide/Show virtual joysticks
    if (MovementJoystick) MovementJoystick->SetVisibility(Visibility);
    if (LookJoystick) LookJoystick->SetVisibility(Visibility);
}

bool UTitanPCMobileHUD::ShouldShowTouchControls() const
{
    // Check if we're on a mobile platform
    FString PlatformName = FPlatformProperties::PlatformName();
    return PlatformName.Contains(TEXT("Android")) || PlatformName.Contains(TEXT("iOS"));
}

void UTitanPCMobileHUD::OnFireButtonPressed()
{
    OnFirePressed.Broadcast();
}

void UTitanPCMobileHUD::OnJumpButtonPressed()
{
    OnJumpPressed.Broadcast();
}

void UTitanPCMobileHUD::OnReloadButtonPressed()
{
    OnReloadPressed.Broadcast();
}

void UTitanPCMobileHUD::OnSprintButtonPressed()
{
    OnSprintPressed.Broadcast();
}

void UTitanPCMobileHUD::OnAimButtonPressed()
{
    OnAimPressed.Broadcast();
}

void UTitanPCMobileHUD::OnMeleeButtonPressed()
{
    OnMeleePressed.Broadcast();
}

void UTitanPCMobileHUD::BindButtonEvents()
{
    // Bind touch control button events
    if (FireButton)
    {
        FireButton->OnPressed.AddDynamic(this, &UTitanPCMobileHUD::OnFireButtonPressed);
    }
    
    if (JumpButton)
    {
        JumpButton->OnPressed.AddDynamic(this, &UTitanPCMobileHUD::OnJumpButtonPressed);
    }
    
    if (ReloadButton)
    {
        ReloadButton->OnPressed.AddDynamic(this, &UTitanPCMobileHUD::OnReloadButtonPressed);
    }
    
    if (SprintButton)
    {
        SprintButton->OnPressed.AddDynamic(this, &UTitanPCMobileHUD::OnSprintButtonPressed);
    }
    
    if (AimButton)
    {
        AimButton->OnPressed.AddDynamic(this, &UTitanPCMobileHUD::OnAimButtonPressed);
    }
    
    if (MeleeButton)
    {
        MeleeButton->OnPressed.AddDynamic(this, &UTitanPCMobileHUD::OnMeleeButtonPressed);
    }
}

void UTitanPCMobileHUD::UpdatePlatformSpecificUI()
{
    bool bShouldShowTouch = ShouldShowTouchControls();
    
    if (bTouchControlsVisible != bShouldShowTouch)
    {
        SetTouchControlsVisible(bShouldShowTouch);
    }
    
    // Adjust UI scale for mobile
    if (bShouldShowTouch)
    {
        // Make UI elements larger for touch
        if (FireButton)
        {
            FireButton->SetRenderScale(FVector2D(1.2f, 1.2f));
        }
        if (JumpButton)
        {
            JumpButton->SetRenderScale(FVector2D(1.2f, 1.2f));
        }
        // Scale other buttons similarly...
    }
    else
    {
        // Standard size for PC
        if (FireButton)
        {
            FireButton->SetRenderScale(FVector2D(1.0f, 1.0f));
        }
        if (JumpButton)
        {
            JumpButton->SetRenderScale(FVector2D(1.0f, 1.0f));
        }
        // Reset other buttons similarly...
    }
}