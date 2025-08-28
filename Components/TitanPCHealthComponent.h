#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "AbilitySystemInterface.h"
#include "TitanPCHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, Health, float, MaxHealth, float, HealthPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStaminaChanged, float, Stamina, float, MaxStamina, float, StaminaPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRevived);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TITANPC_API UTitanPCHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTitanPCHealthComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Health Management
    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(float DamageAmount, AActor* DamageDealer = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float HealAmount);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetHealth(float NewHealth);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetMaxHealth(float NewMaxHealth);

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercentage() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsAlive() const { return CurrentHealth > 0.0f; }

    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsAtFullHealth() const { return FMath::IsNearlyEqual(CurrentHealth, MaxHealth); }

    // Stamina Management
    UFUNCTION(BlueprintCallable, Category = "Stamina")
    bool ConsumeStamina(float StaminaAmount);

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void RestoreStamina(float StaminaAmount);

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void SetStamina(float NewStamina);

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void SetMaxStamina(float NewMaxStamina);

    UFUNCTION(BlueprintPure, Category = "Stamina")
    float GetStamina() const { return CurrentStamina; }

    UFUNCTION(BlueprintPure, Category = "Stamina")
    float GetMaxStamina() const { return MaxStamina; }

    UFUNCTION(BlueprintPure, Category = "Stamina")
    float GetStaminaPercentage() const;

    UFUNCTION(BlueprintPure, Category = "Stamina")
    bool IsAtFullStamina() const { return FMath::IsNearlyEqual(CurrentStamina, MaxStamina); }

    UFUNCTION(BlueprintPure, Category = "Stamina")
    bool HasEnoughStamina(float RequiredStamina) const { return CurrentStamina >= RequiredStamina; }

    // Regeneration
    UFUNCTION(BlueprintCallable, Category = "Regeneration")
    void EnableHealthRegeneration(bool bEnable = true);

    UFUNCTION(BlueprintCallable, Category = "Regeneration")
    void EnableStaminaRegeneration(bool bEnable = true);

    UFUNCTION(BlueprintCallable, Category = "Regeneration")
    void PauseStaminaRegeneration(float Duration);

    // Death and Revival
    UFUNCTION(BlueprintCallable, Category = "Death")
    void Kill();

    UFUNCTION(BlueprintCallable, Category = "Death")
    void Revive(float ReviveHealthPercentage = 0.5f);

    // Status Effects
    UFUNCTION(BlueprintCallable, Category = "Status")
    void SetInvulnerable(bool bInvulnerable);

    UFUNCTION(BlueprintPure, Category = "Status")
    bool IsInvulnerable() const { return bIsInvulnerable; }

    // Shield System
    UFUNCTION(BlueprintCallable, Category = "Shield")
    void SetShield(float NewShield);

    UFUNCTION(BlueprintCallable, Category = "Shield")
    void SetMaxShield(float NewMaxShield);

    UFUNCTION(BlueprintPure, Category = "Shield")
    float GetShield() const { return CurrentShield; }

    UFUNCTION(BlueprintPure, Category = "Shield")
    float GetMaxShield() const { return MaxShield; }

    UFUNCTION(BlueprintPure, Category = "Shield")
    bool HasShield() const { return CurrentShield > 0.0f; }

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Stamina")
    FOnStaminaChanged OnStaminaChanged;

    UPROPERTY(BlueprintAssignable, Category = "Death")
    FOnDeath OnDeath;

    UPROPERTY(BlueprintAssignable, Category = "Death")
    FOnRevived OnRevived;

protected:
    void UpdateHealthRegeneration(float DeltaTime);
    void UpdateStaminaRegeneration(float DeltaTime);
    void BroadcastHealthChanged();
    void BroadcastStaminaChanged();
    void HandleDeath();

private:
    // Health Properties
    UPROPERTY(EditDefaultsOnly, Category = "Health", meta = (ClampMin = "0.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Health")
    float CurrentHealth = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Health")
    bool bHealthRegenerationEnabled = true;

    UPROPERTY(EditDefaultsOnly, Category = "Health", meta = (ClampMin = "0.0"))
    float HealthRegenerationRate = 5.0f; // HP per second

    UPROPERTY(EditDefaultsOnly, Category = "Health", meta = (ClampMin = "0.0"))
    float HealthRegenerationDelay = 5.0f; // Seconds after taking damage

    float LastDamageTime = 0.0f;

    // Stamina Properties
    UPROPERTY(EditDefaultsOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
    float MaxStamina = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Stamina")
    float CurrentStamina = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Stamina")
    bool bStaminaRegenerationEnabled = true;

    UPROPERTY(EditDefaultsOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
    float StaminaRegenerationRate = 20.0f; // Stamina per second

    UPROPERTY(EditDefaultsOnly, Category = "Stamina", meta = (ClampMin = "0.0"))
    float StaminaRegenerationDelay = 1.0f; // Seconds after consuming stamina

    float LastStaminaConsumeTime = 0.0f;
    float StaminaRegenPausedUntil = 0.0f;

    // Shield Properties
    UPROPERTY(EditDefaultsOnly, Category = "Shield", meta = (ClampMin = "0.0"))
    float MaxShield = 50.0f;

    UPROPERTY(VisibleAnywhere, Category = "Shield")
    float CurrentShield = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Shield")
    float ShieldRegenerationRate = 10.0f; // Shield per second

    UPROPERTY(EditDefaultsOnly, Category = "Shield")
    float ShieldRegenerationDelay = 3.0f; // Seconds after taking damage

    // Status Properties
    UPROPERTY(VisibleAnywhere, Category = "Status")
    bool bIsAlive = true;

    UPROPERTY(VisibleAnywhere, Category = "Status")
    bool bIsInvulnerable = false;

    // Damage Reduction
    UPROPERTY(EditDefaultsOnly, Category = "Defense", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DamageReduction = 0.0f; // Percentage of damage reduced (0.0 = no reduction, 1.0 = immune)

    // Critical Hit System
    UPROPERTY(EditDefaultsOnly, Category = "Defense")
    bool bCanTakeCriticalHits = true;

    UPROPERTY(EditDefaultsOnly, Category = "Defense")
    float CriticalHitMultiplier = 2.0f;
};