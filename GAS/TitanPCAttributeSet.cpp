#include "TitanPCAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UTitanPCAttributeSet::UTitanPCAttributeSet()
{
    // Initialize default values
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    InitHealthRegenRate(5.0f);
    
    InitStamina(100.0f);
    InitMaxStamina(100.0f);
    InitStaminaRegenRate(20.0f);
    
    InitMovementSpeed(500.0f);
    InitSprintSpeed(800.0f);
    
    InitAttackPower(50.0f);
    InitDefensePower(10.0f);
    InitCriticalHitChance(0.05f); // 5%
    InitCriticalHitMultiplier(2.0f);
    
    InitCharacterLevel(1.0f);
    InitExperiencePoints(0.0f);
    InitSkillPoints(0.0f);
    
    InitShield(0.0f);
    InitMaxShield(50.0f);
    InitShieldRegenRate(10.0f);
}

void UTitanPCAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, SprintSpeed, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, DefensePower, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, CriticalHitMultiplier, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, CharacterLevel, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, ExperiencePoints, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, SkillPoints, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, Shield, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UTitanPCAttributeSet, ShieldRegenRate, COND_None, REPNOTIFY_Always);
}

void UTitanPCAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // Clamp attributes to valid ranges
    if (Attribute == GetMaxHealthAttribute())
    {
        NewValue = FMath::Max(NewValue, 1.0f);
    }
    else if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetMaxStaminaAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);
    }
    else if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
    }
    else if (Attribute == GetMaxShieldAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);
    }
    else if (Attribute == GetShieldAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxShield());
    }
    else if (Attribute == GetMovementSpeedAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);
    }
    else if (Attribute == GetSprintSpeedAttribute())
    {
        NewValue = FMath::Max(NewValue, GetMovementSpeed());
    }
    else if (Attribute == GetCharacterLevelAttribute())
    {
        NewValue = FMath::Max(NewValue, 1.0f);
    }
    else if (Attribute == GetExperiencePointsAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);
    }
    else if (Attribute == GetSkillPointsAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);
    }
    else if (Attribute == GetCriticalHitChanceAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f); // 0-100%
    }
    else if (Attribute == GetCriticalHitMultiplierAttribute())
    {
        NewValue = FMath::Max(NewValue, 1.0f);
    }
}

void UTitanPCAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
    UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
    const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

    // Get the target actor
    AActor* TargetActor = nullptr;
    AController* TargetController = nullptr;
    if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
        TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
        TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
    }

    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        // Handle damage
        const float LocalDamageDone = GetDamage();
        SetDamage(0.0f);

        if (LocalDamageDone > 0)
        {
            // Apply damage to shield first, then health
            const float OldShield = GetShield();
            const float OldHealth = GetHealth();

            if (OldShield > 0.0f)
            {
                // Damage shield first
                const float ShieldDamage = FMath::Min(LocalDamageDone, OldShield);
                SetShield(FMath::Max(OldShield - ShieldDamage, 0.0f));
                
                // Apply remaining damage to health
                const float RemainingDamage = LocalDamageDone - ShieldDamage;
                if (RemainingDamage > 0.0f)
                {
                    SetHealth(FMath::Max(OldHealth - RemainingDamage, 0.0f));
                }
            }
            else
            {
                // No shield, damage health directly
                SetHealth(FMath::Max(OldHealth - LocalDamageDone, 0.0f));
            }

            // Handle death
            if (GetHealth() <= 0.0f && TargetActor)
            {
                // TODO: Handle character death
                UE_LOG(LogTemp, Warning, TEXT("Character %s has died!"), *TargetActor->GetName());
            }
        }
    }
    else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
    {
        // Handle healing
        const float LocalHealingDone = GetHealing();
        SetHealing(0.0f);

        if (LocalHealingDone > 0)
        {
            SetHealth(FMath::Min(GetHealth() + LocalHealingDone, GetMaxHealth()));
        }
    }
    else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }
    else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
    {
        SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
    }
    else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
    {
        SetShield(FMath::Clamp(GetShield(), 0.0f, GetMaxShield()));
    }
}

void UTitanPCAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, Health, OldHealth);
}

void UTitanPCAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, MaxHealth, OldMaxHealth);
}

void UTitanPCAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UTitanPCAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, Stamina, OldStamina);
}

void UTitanPCAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, MaxStamina, OldMaxStamina);
}

void UTitanPCAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, StaminaRegenRate, OldStaminaRegenRate);
}

void UTitanPCAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, MovementSpeed, OldMovementSpeed);
}

void UTitanPCAttributeSet::OnRep_SprintSpeed(const FGameplayAttributeData& OldSprintSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, SprintSpeed, OldSprintSpeed);
}

void UTitanPCAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, AttackPower, OldAttackPower);
}

void UTitanPCAttributeSet::OnRep_DefensePower(const FGameplayAttributeData& OldDefensePower)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, DefensePower, OldDefensePower);
}

void UTitanPCAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UTitanPCAttributeSet::OnRep_CriticalHitMultiplier(const FGameplayAttributeData& OldCriticalHitMultiplier)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, CriticalHitMultiplier, OldCriticalHitMultiplier);
}

void UTitanPCAttributeSet::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, CharacterLevel, OldCharacterLevel);
}

void UTitanPCAttributeSet::OnRep_ExperiencePoints(const FGameplayAttributeData& OldExperiencePoints)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, ExperiencePoints, OldExperiencePoints);
}

void UTitanPCAttributeSet::OnRep_SkillPoints(const FGameplayAttributeData& OldSkillPoints)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, SkillPoints, OldSkillPoints);
}

void UTitanPCAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, Shield, OldShield);
}

void UTitanPCAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, MaxShield, OldMaxShield);
}

void UTitanPCAttributeSet::OnRep_ShieldRegenRate(const FGameplayAttributeData& OldShieldRegenRate)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UTitanPCAttributeSet, ShieldRegenRate, OldShieldRegenRate);
}

void UTitanPCAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
    UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponent();
    const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
    if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilitySystemComponent)
    {
        // Change current value to maintain the current Val / Max percent
        const float CurrentValue = AffectedAttribute.GetCurrentValue();
        float NewDelta = (CurrentMaxValue > 0.0f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

        AbilitySystemComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
    }
}