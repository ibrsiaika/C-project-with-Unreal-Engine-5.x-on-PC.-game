#include "TitanPCHealthComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UTitanPCHealthComponent::UTitanPCHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize values
    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;
    CurrentShield = 0.0f; // Start without shield
    bIsAlive = true;
    bIsInvulnerable = false;
}

void UTitanPCHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize current values to max values
    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;
    
    BroadcastHealthChanged();
    BroadcastStaminaChanged();
}

void UTitanPCHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (!bIsAlive) return;
    
    UpdateHealthRegeneration(DeltaTime);
    UpdateStaminaRegeneration(DeltaTime);
}

void UTitanPCHealthComponent::TakeDamage(float DamageAmount, AActor* DamageDealer)
{
    if (!bIsAlive || bIsInvulnerable || DamageAmount <= 0.0f)
    {
        return;
    }
    
    // Apply damage reduction
    float FinalDamage = DamageAmount * (1.0f - DamageReduction);
    
    // Handle shield absorption
    if (CurrentShield > 0.0f)
    {
        float ShieldDamage = FMath::Min(FinalDamage, CurrentShield);
        CurrentShield -= ShieldDamage;
        FinalDamage -= ShieldDamage;
        
        if (CurrentShield < 0.0f)
        {
            CurrentShield = 0.0f;
        }
    }
    
    // Apply remaining damage to health
    if (FinalDamage > 0.0f)
    {
        CurrentHealth = FMath::Max(0.0f, CurrentHealth - FinalDamage);
        LastDamageTime = GetWorld()->GetTimeSeconds();
    }
    
    BroadcastHealthChanged();
    
    // Check for death
    if (CurrentHealth <= 0.0f && bIsAlive)
    {
        HandleDeath();
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Took %f damage. Health: %f/%f, Shield: %f/%f"), 
        DamageAmount, CurrentHealth, MaxHealth, CurrentShield, MaxShield);
}

void UTitanPCHealthComponent::Heal(float HealAmount)
{
    if (!bIsAlive || HealAmount <= 0.0f)
    {
        return;
    }
    
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
    
    if (!FMath::IsNearlyEqual(OldHealth, CurrentHealth))
    {
        BroadcastHealthChanged();
        UE_LOG(LogTemp, Warning, TEXT("Healed %f health. Health: %f/%f"), HealAmount, CurrentHealth, MaxHealth);
    }
}

void UTitanPCHealthComponent::SetHealth(float NewHealth)
{
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    
    if (!FMath::IsNearlyEqual(OldHealth, CurrentHealth))
    {
        BroadcastHealthChanged();
        
        // Check for death or revival
        if (CurrentHealth <= 0.0f && bIsAlive)
        {
            HandleDeath();
        }
        else if (CurrentHealth > 0.0f && !bIsAlive)
        {
            bIsAlive = true;
            OnRevived.Broadcast();
        }
    }
}

void UTitanPCHealthComponent::SetMaxHealth(float NewMaxHealth)
{
    if (NewMaxHealth > 0.0f)
    {
        float HealthRatio = MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 1.0f;
        MaxHealth = NewMaxHealth;
        CurrentHealth = MaxHealth * HealthRatio;
        BroadcastHealthChanged();
    }
}

float UTitanPCHealthComponent::GetHealthPercentage() const
{
    return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}

bool UTitanPCHealthComponent::ConsumeStamina(float StaminaAmount)
{
    if (!bIsAlive || StaminaAmount <= 0.0f)
    {
        return false;
    }
    
    if (CurrentStamina < StaminaAmount)
    {
        return false; // Not enough stamina
    }
    
    CurrentStamina = FMath::Max(0.0f, CurrentStamina - StaminaAmount);
    LastStaminaConsumeTime = GetWorld()->GetTimeSeconds();
    
    BroadcastStaminaChanged();
    return true;
}

void UTitanPCHealthComponent::RestoreStamina(float StaminaAmount)
{
    if (!bIsAlive || StaminaAmount <= 0.0f)
    {
        return;
    }
    
    float OldStamina = CurrentStamina;
    CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + StaminaAmount);
    
    if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
    {
        BroadcastStaminaChanged();
    }
}

void UTitanPCHealthComponent::SetStamina(float NewStamina)
{
    float OldStamina = CurrentStamina;
    CurrentStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
    
    if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
    {
        BroadcastStaminaChanged();
    }
}

void UTitanPCHealthComponent::SetMaxStamina(float NewMaxStamina)
{
    if (NewMaxStamina > 0.0f)
    {
        float StaminaRatio = MaxStamina > 0.0f ? CurrentStamina / MaxStamina : 1.0f;
        MaxStamina = NewMaxStamina;
        CurrentStamina = MaxStamina * StaminaRatio;
        BroadcastStaminaChanged();
    }
}

float UTitanPCHealthComponent::GetStaminaPercentage() const
{
    return MaxStamina > 0.0f ? CurrentStamina / MaxStamina : 0.0f;
}

void UTitanPCHealthComponent::EnableHealthRegeneration(bool bEnable)
{
    bHealthRegenerationEnabled = bEnable;
}

void UTitanPCHealthComponent::EnableStaminaRegeneration(bool bEnable)
{
    bStaminaRegenerationEnabled = bEnable;
}

void UTitanPCHealthComponent::PauseStaminaRegeneration(float Duration)
{
    if (Duration > 0.0f)
    {
        StaminaRegenPausedUntil = GetWorld()->GetTimeSeconds() + Duration;
    }
}

void UTitanPCHealthComponent::Kill()
{
    if (bIsAlive)
    {
        CurrentHealth = 0.0f;
        HandleDeath();
        BroadcastHealthChanged();
    }
}

void UTitanPCHealthComponent::Revive(float ReviveHealthPercentage)
{
    if (!bIsAlive)
    {
        ReviveHealthPercentage = FMath::Clamp(ReviveHealthPercentage, 0.1f, 1.0f);
        CurrentHealth = MaxHealth * ReviveHealthPercentage;
        CurrentStamina = MaxStamina;
        bIsAlive = true;
        
        BroadcastHealthChanged();
        BroadcastStaminaChanged();
        OnRevived.Broadcast();
        
        UE_LOG(LogTemp, Warning, TEXT("Character revived with %f%% health"), ReviveHealthPercentage * 100.0f);
    }
}

void UTitanPCHealthComponent::SetInvulnerable(bool bInvulnerable)
{
    bIsInvulnerable = bInvulnerable;
    UE_LOG(LogTemp, Warning, TEXT("Invulnerability set to: %s"), bIsInvulnerable ? TEXT("True") : TEXT("False"));
}

void UTitanPCHealthComponent::SetShield(float NewShield)
{
    CurrentShield = FMath::Clamp(NewShield, 0.0f, MaxShield);
}

void UTitanPCHealthComponent::SetMaxShield(float NewMaxShield)
{
    if (NewMaxShield > 0.0f)
    {
        float ShieldRatio = MaxShield > 0.0f ? CurrentShield / MaxShield : 0.0f;
        MaxShield = NewMaxShield;
        CurrentShield = MaxShield * ShieldRatio;
    }
}

void UTitanPCHealthComponent::UpdateHealthRegeneration(float DeltaTime)
{
    if (!bHealthRegenerationEnabled || IsAtFullHealth() || !bIsAlive)
    {
        return;
    }
    
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastDamageTime >= HealthRegenerationDelay)
    {
        float RegenAmount = HealthRegenerationRate * DeltaTime;
        float OldHealth = CurrentHealth;
        CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + RegenAmount);
        
        if (!FMath::IsNearlyEqual(OldHealth, CurrentHealth))
        {
            BroadcastHealthChanged();
        }
    }
    
    // Regenerate shield
    if (CurrentShield < MaxShield && CurrentTime - LastDamageTime >= ShieldRegenerationDelay)
    {
        float ShieldRegenAmount = ShieldRegenerationRate * DeltaTime;
        CurrentShield = FMath::Min(MaxShield, CurrentShield + ShieldRegenAmount);
    }
}

void UTitanPCHealthComponent::UpdateStaminaRegeneration(float DeltaTime)
{
    if (!bStaminaRegenerationEnabled || IsAtFullStamina() || !bIsAlive)
    {
        return;
    }
    
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    // Check if stamina regeneration is paused
    if (CurrentTime < StaminaRegenPausedUntil)
    {
        return;
    }
    
    if (CurrentTime - LastStaminaConsumeTime >= StaminaRegenerationDelay)
    {
        float RegenAmount = StaminaRegenerationRate * DeltaTime;
        float OldStamina = CurrentStamina;
        CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + RegenAmount);
        
        if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
        {
            BroadcastStaminaChanged();
        }
    }
}

void UTitanPCHealthComponent::BroadcastHealthChanged()
{
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetHealthPercentage());
}

void UTitanPCHealthComponent::BroadcastStaminaChanged()
{
    OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina, GetStaminaPercentage());
}

void UTitanPCHealthComponent::HandleDeath()
{
    bIsAlive = false;
    OnDeath.Broadcast();
    
    UE_LOG(LogTemp, Warning, TEXT("Character has died"));
    
    // Stop all regeneration on death
    bHealthRegenerationEnabled = false;
    bStaminaRegenerationEnabled = false;
}