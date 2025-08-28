
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "TitanPCCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UAbilitySystemComponent;
class UTitanPCAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class UTitanPCMovementComponent;
class UTitanPCHealthComponent;
class UTitanPCWeaponComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Health, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, Stamina, float, MaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, NewLevel);

UCLASS()
class TITANPC_API ATitanPCCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ATitanPCCharacter();

    // IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;

    // Enhanced Movement System
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void JumpPressed();
    void JumpReleased();
    void SprintPressed();
    void SprintReleased();
    void CrouchPressed();
    void SlidePressed();
    void WallRunPressed();
    void DashPressed();
    
    // Combat System
    void FirePressed();
    void FireReleased();
    void AimPressed();
    void AimReleased();
    void ReloadPressed();
    void MeleePressed();
    
    // Interaction System
    void InteractPressed();
    void InventoryPressed();
    
    // Ability System
    void AbilityInputPressed(int32 InputID);
    void AbilityInputReleased(int32 InputID);

    // Gameplay Ability System Setup
    void InitializeAbilitySystem();
    void GrantStartupAbilities();
    void BindAbilityActivationToInputComponent();

    // Attribute change callbacks
    virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
    virtual void OnStaminaAttributeChanged(const FOnAttributeChangeData& Data);
    virtual void OnLevelAttributeChanged(const FOnAttributeChangeData& Data);

private:
    // Camera Components
    UPROPERTY(VisibleAnywhere, Category="Camera")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, Category="Camera")
    UCameraComponent* Camera;

    // Enhanced Movement Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
    UTitanPCMovementComponent* EnhancedMovementComponent;

    // Health Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health", meta=(AllowPrivateAccess="true"))
    UTitanPCHealthComponent* HealthComponent;

    // Weapon Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
    UTitanPCWeaponComponent* WeaponComponent;

    // Ability System Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities", meta=(AllowPrivateAccess="true"))
    UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UTitanPCAttributeSet* AttributeSet;

    // Input Actions
    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputMappingContext* MappingContext;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Move;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Look;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Jump;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Sprint;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Crouch;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Slide;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_WallRun;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Dash;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Fire;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Aim;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Reload;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Melee;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Interact;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Inventory;

    // Ability Input Actions
    UPROPERTY(EditDefaultsOnly, Category="Input|Abilities")
    UInputAction* IA_Ability1;

    UPROPERTY(EditDefaultsOnly, Category="Input|Abilities")
    UInputAction* IA_Ability2;

    UPROPERTY(EditDefaultsOnly, Category="Input|Abilities")
    UInputAction* IA_Ability3;

    UPROPERTY(EditDefaultsOnly, Category="Input|Abilities")
    UInputAction* IA_Ability4;

    // Movement Settings
    UPROPERTY(EditAnywhere, Category="Movement")
    float WalkSpeed = 500.f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float SprintSpeed = 800.f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float CrouchSpeed = 200.f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float SlideSpeed = 1200.f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float WallRunSpeed = 700.f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float DashDistance = 1000.f;

    // Startup Abilities and Effects
    UPROPERTY(EditDefaultsOnly, Category="Abilities")
    TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

    UPROPERTY(EditDefaultsOnly, Category="Abilities")
    TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

    // Gameplay Tags
    UPROPERTY(EditDefaultsOnly, Category="GameplayTags")
    FGameplayTagContainer StartupTags;

    // Character State
    bool bIsAiming = false;
    bool bIsSprinting = false;
    bool bIsSliding = false;
    bool bIsWallRunning = false;
    bool bCanDash = true;

    // Dash cooldown timer
    FTimerHandle DashCooldownTimer;
    float DashCooldownTime = 2.0f;

public:
    // Events
    UPROPERTY(BlueprintAssignable, Category="Character")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category="Character")
    FOnStaminaChanged OnStaminaChanged;

    UPROPERTY(BlueprintAssignable, Category="Character")
    FOnLevelChanged OnLevelChanged;

    // Getters
    UFUNCTION(BlueprintPure, Category="Character")
    float GetHealth() const;

    UFUNCTION(BlueprintPure, Category="Character")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintPure, Category="Character")
    float GetStamina() const;

    UFUNCTION(BlueprintPure, Category="Character")
    float GetMaxStamina() const;

    UFUNCTION(BlueprintPure, Category="Character")
    int32 GetCharacterLevel() const;

    UFUNCTION(BlueprintPure, Category="Character")
    bool IsAiming() const { return bIsAiming; }

    UFUNCTION(BlueprintPure, Category="Character")
    bool IsSprinting() const { return bIsSprinting; }

    UFUNCTION(BlueprintPure, Category="Movement")
    bool IsSliding() const { return bIsSliding; }

    UFUNCTION(BlueprintPure, Category="Movement")
    bool IsWallRunning() const { return bIsWallRunning; }

    // Component Accessors
    UFUNCTION(BlueprintPure, Category="Components")
    UTitanPCMovementComponent* GetEnhancedMovementComponent() const { return EnhancedMovementComponent; }

    UFUNCTION(BlueprintPure, Category="Components")
    UTitanPCHealthComponent* GetHealthComponent() const { return HealthComponent; }

    UFUNCTION(BlueprintPure, Category="Components")
    UTitanPCWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

    // Ability System Public Interface
    UFUNCTION(BlueprintCallable, Category="Abilities")
    bool TryActivateAbilityByTag(FGameplayTag AbilityTag);

    UFUNCTION(BlueprintCallable, Category="Abilities")
    void CancelAbilityByTag(FGameplayTag AbilityTag);

protected:
    void ResetDashCooldown();
};
