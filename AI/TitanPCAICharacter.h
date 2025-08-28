#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "TitanPCAICharacter.generated.h"

class UAbilitySystemComponent;
class UTitanPCAttributeSet;
class UTitanPCHealthComponent;
class UTitanPCWeaponComponent;
class UBehaviorTree;
class UBlackboardComponent;

UENUM(BlueprintType)
enum class ETitanPCAIState : uint8
{
    Passive     UMETA(DisplayName = "Passive"),
    Patrolling  UMETA(DisplayName = "Patrolling"),
    Investigating UMETA(DisplayName = "Investigating"),
    Chasing     UMETA(DisplayName = "Chasing"),
    Combat      UMETA(DisplayName = "Combat"),
    Fleeing     UMETA(DisplayName = "Fleeing"),
    Dead        UMETA(DisplayName = "Dead")
};

UCLASS()
class TITANPC_API ATitanPCAICharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ATitanPCAICharacter();

    // IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual void PossessedBy(AController* NewController) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // AI State Management
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetAIState(ETitanPCAIState NewState);

    UFUNCTION(BlueprintPure, Category = "AI")
    ETitanPCAIState GetAIState() const { return CurrentAIState; }

    // Combat Functions
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartCombat(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopCombat();

    UFUNCTION(BlueprintPure, Category = "Combat")
    AActor* GetCombatTarget() const { return CombatTarget; }

    // Perception
    UFUNCTION(BlueprintCallable, Category = "Perception")
    bool CanSeeActor(AActor* Actor) const;

    UFUNCTION(BlueprintPure, Category = "Perception")
    float GetSightRange() const { return SightRange; }

    UFUNCTION(BlueprintPure, Category = "Perception")
    float GetHearingRange() const { return HearingRange; }

    // Health Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Health")
    void OnHealthChanged(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category = "Health")
    void OnDeath();

    // Component Accessors
    UFUNCTION(BlueprintPure, Category = "Components")
    UTitanPCHealthComponent* GetHealthComponent() const { return HealthComponent; }

    UFUNCTION(BlueprintPure, Category = "Components")
    UTitanPCWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

protected:
    void InitializeAIComponents();
    void SetupBehaviorTree();

    UFUNCTION()
    void HandleHealthChanged(float Health, float MaxHealth, float HealthPercentage);

    UFUNCTION()
    void HandleDeath();

private:
    // AI Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    class UAIPerceptionComponent* AIPerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
    UTitanPCHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    UTitanPCWeaponComponent* WeaponComponent;

    // Ability System Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UTitanPCAttributeSet* AttributeSet;

    // AI Configuration
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTree;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float SightRange = 1200.0f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float SightAngle = 90.0f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float HearingRange = 800.0f;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    float PatrolRadius = 500.0f;

    // AI State
    ETitanPCAIState CurrentAIState = ETitanPCAIState::Passive;
    ETitanPCAIState PreviousAIState = ETitanPCAIState::Passive;

    UPROPERTY()
    AActor* CombatTarget;

    UPROPERTY()
    FVector HomeLocation;

    // Combat Settings
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float CombatRange = 800.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float MeleeRange = 150.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    bool bCanUseRangedWeapons = true;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    bool bCanUseMelee = true;

    // AI Personality
    UPROPERTY(EditDefaultsOnly, Category = "AI Personality")
    float Aggressiveness = 0.5f; // 0 = Passive, 1 = Very Aggressive

    UPROPERTY(EditDefaultsOnly, Category = "AI Personality")
    float Alertness = 0.7f; // How quickly they detect threats

    UPROPERTY(EditDefaultsOnly, Category = "AI Personality")
    float Courage = 0.6f; // Likelihood to fight vs flee when low on health
};