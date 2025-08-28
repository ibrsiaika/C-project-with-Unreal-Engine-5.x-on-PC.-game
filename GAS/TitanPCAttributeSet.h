#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "TitanPCAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TITANPC_API UTitanPCAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UTitanPCAttributeSet();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    // Health Attributes
    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, MaxHealth)

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegenRate)
    FGameplayAttributeData HealthRegenRate;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, HealthRegenRate)

    // Stamina Attributes
    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, Stamina)

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxStamina)
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, MaxStamina)

    UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_StaminaRegenRate)
    FGameplayAttributeData StaminaRegenRate;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, StaminaRegenRate)

    // Movement Attributes
    UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_MovementSpeed)
    FGameplayAttributeData MovementSpeed;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, MovementSpeed)

    UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_SprintSpeed)
    FGameplayAttributeData SprintSpeed;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, SprintSpeed)

    // Combat Attributes
    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_AttackPower)
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, AttackPower)

    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_DefensePower)
    FGameplayAttributeData DefensePower;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, DefensePower)

    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_CriticalHitChance)
    FGameplayAttributeData CriticalHitChance;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, CriticalHitChance)

    UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_CriticalHitMultiplier)
    FGameplayAttributeData CriticalHitMultiplier;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, CriticalHitMultiplier)

    // Character Progression
    UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_CharacterLevel)
    FGameplayAttributeData CharacterLevel;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, CharacterLevel)

    UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_ExperiencePoints)
    FGameplayAttributeData ExperiencePoints;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, ExperiencePoints)

    UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_SkillPoints)
    FGameplayAttributeData SkillPoints;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, SkillPoints)

    // Shield Attributes
    UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_Shield)
    FGameplayAttributeData Shield;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, Shield)

    UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_MaxShield)
    FGameplayAttributeData MaxShield;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, MaxShield)

    UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_ShieldRegenRate)
    FGameplayAttributeData ShieldRegenRate;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, ShieldRegenRate)

    // Meta attributes (used for calculations, not stored)
    UPROPERTY(BlueprintReadOnly, Category = "Meta")
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, Damage)

    UPROPERTY(BlueprintReadOnly, Category = "Meta")
    FGameplayAttributeData Healing;
    ATTRIBUTE_ACCESSORS(UTitanPCAttributeSet, Healing)

protected:
    // Rep notifies
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);

    UFUNCTION()
    virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

    UFUNCTION()
    virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

    UFUNCTION()
    virtual void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate);

    UFUNCTION()
    virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);

    UFUNCTION()
    virtual void OnRep_SprintSpeed(const FGameplayAttributeData& OldSprintSpeed);

    UFUNCTION()
    virtual void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);

    UFUNCTION()
    virtual void OnRep_DefensePower(const FGameplayAttributeData& OldDefensePower);

    UFUNCTION()
    virtual void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance);

    UFUNCTION()
    virtual void OnRep_CriticalHitMultiplier(const FGameplayAttributeData& OldCriticalHitMultiplier);

    UFUNCTION()
    virtual void OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel);

    UFUNCTION()
    virtual void OnRep_ExperiencePoints(const FGameplayAttributeData& OldExperiencePoints);

    UFUNCTION()
    virtual void OnRep_SkillPoints(const FGameplayAttributeData& OldSkillPoints);

    UFUNCTION()
    virtual void OnRep_Shield(const FGameplayAttributeData& OldShield);

    UFUNCTION()
    virtual void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);

    UFUNCTION()
    virtual void OnRep_ShieldRegenRate(const FGameplayAttributeData& OldShieldRegenRate);

    // Helper functions
    void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
};