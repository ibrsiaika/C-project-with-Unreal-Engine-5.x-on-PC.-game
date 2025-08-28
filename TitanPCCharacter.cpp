
#include "TitanPCCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/GameplayAbility.h"
#include "Engine/Engine.h"
#include "Components/TitanPCMovementComponent.h"
#include "Components/TitanPCHealthComponent.h"
#include "Components/TitanPCWeaponComponent.h"
#include "GAS/TitanPCAttributeSet.h"

ATitanPCCharacter::ATitanPCCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create Ability System Component
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Create Attribute Set
    AttributeSet = CreateDefaultSubobject<UTitanPCAttributeSet>(TEXT("AttributeSet"));

    // Setup Camera System
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 350.f;
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    // Replace default movement component with enhanced one
    if (GetCharacterMovement())
    {
        DestroyComponent(GetCharacterMovement());
    }
    
    EnhancedMovementComponent = CreateDefaultSubobject<UTitanPCMovementComponent>(TEXT("EnhancedMovementComponent"));
    EnhancedMovementComponent->UpdatedComponent = RootComponent;

    // Create Health Component
    HealthComponent = CreateDefaultSubobject<UTitanPCHealthComponent>(TEXT("HealthComponent"));

    // Create Weapon Component
    WeaponComponent = CreateDefaultSubobject<UTitanPCWeaponComponent>(TEXT("WeaponComponent"));

    // Setup movement defaults
    EnhancedMovementComponent->MaxWalkSpeed = WalkSpeed;
    bUseControllerRotationYaw = false;
    EnhancedMovementComponent->bOrientRotationToMovement = true;
    
    // Setup replication
    bReplicates = true;
    SetReplicateMovement(true);
}

UAbilitySystemComponent* ATitanPCCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ATitanPCCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        InitializeAbilitySystem();
    }
}

void ATitanPCCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void ATitanPCCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (MappingContext) Subsystem->AddMappingContext(MappingContext, 0);
        }
    }

    // Connect stamina component to movement
    if (EnhancedMovementComponent && HealthComponent)
    {
        EnhancedMovementComponent->SetStaminaComponent(HealthComponent);
    }

    // Bind attribute change callbacks
    if (AbilitySystemComponent && AttributeSet)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ATitanPCCharacter::OnHealthAttributeChanged);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute()).AddUObject(this, &ATitanPCCharacter::OnStaminaAttributeChanged);
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCharacterLevelAttribute()).AddUObject(this, &ATitanPCCharacter::OnLevelAttributeChanged);
    }
}

void ATitanPCCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update movement speeds from attributes
    if (AttributeSet && EnhancedMovementComponent)
    {
        EnhancedMovementComponent->MaxWalkSpeed = bIsSprinting ? 
            AttributeSet->GetSprintSpeed() : AttributeSet->GetMovementSpeed();
    }
}

void ATitanPCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Movement
        if (IA_Move) EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATitanPCCharacter::Move);
        if (IA_Look) EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ATitanPCCharacter::Look);
        
        // Basic Actions
        if (IA_Jump) 
        {
            EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &ATitanPCCharacter::JumpPressed);
            EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ATitanPCCharacter::JumpReleased);
        }
        
        // Enhanced Movement
        if (IA_Sprint) 
        {
            EIC->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ATitanPCCharacter::SprintPressed);
            EIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ATitanPCCharacter::SprintReleased);
        }
        if (IA_Crouch) EIC->BindAction(IA_Crouch, ETriggerEvent::Started, this, &ATitanPCCharacter::CrouchPressed);
        if (IA_Slide) EIC->BindAction(IA_Slide, ETriggerEvent::Started, this, &ATitanPCCharacter::SlidePressed);
        if (IA_WallRun) EIC->BindAction(IA_WallRun, ETriggerEvent::Started, this, &ATitanPCCharacter::WallRunPressed);
        if (IA_Dash) EIC->BindAction(IA_Dash, ETriggerEvent::Started, this, &ATitanPCCharacter::DashPressed);
        
        // Combat
        if (IA_Fire)
        {
            EIC->BindAction(IA_Fire, ETriggerEvent::Started, this, &ATitanPCCharacter::FirePressed);
            EIC->BindAction(IA_Fire, ETriggerEvent::Completed, this, &ATitanPCCharacter::FireReleased);
        }
        if (IA_Aim)
        {
            EIC->BindAction(IA_Aim, ETriggerEvent::Started, this, &ATitanPCCharacter::AimPressed);
            EIC->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ATitanPCCharacter::AimReleased);
        }
        if (IA_Reload) EIC->BindAction(IA_Reload, ETriggerEvent::Started, this, &ATitanPCCharacter::ReloadPressed);
        if (IA_Melee) EIC->BindAction(IA_Melee, ETriggerEvent::Started, this, &ATitanPCCharacter::MeleePressed);
        
        // Interaction
        if (IA_Interact) EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &ATitanPCCharacter::InteractPressed);
        if (IA_Inventory) EIC->BindAction(IA_Inventory, ETriggerEvent::Started, this, &ATitanPCCharacter::InventoryPressed);
        
        // Abilities
        if (IA_Ability1) 
        {
            EIC->BindAction(IA_Ability1, ETriggerEvent::Started, this, &ATitanPCCharacter::AbilityInputPressed, 0);
            EIC->BindAction(IA_Ability1, ETriggerEvent::Completed, this, &ATitanPCCharacter::AbilityInputReleased, 0);
        }
        if (IA_Ability2) 
        {
            EIC->BindAction(IA_Ability2, ETriggerEvent::Started, this, &ATitanPCCharacter::AbilityInputPressed, 1);
            EIC->BindAction(IA_Ability2, ETriggerEvent::Completed, this, &ATitanPCCharacter::AbilityInputReleased, 1);
        }
        if (IA_Ability3) 
        {
            EIC->BindAction(IA_Ability3, ETriggerEvent::Started, this, &ATitanPCCharacter::AbilityInputPressed, 2);
            EIC->BindAction(IA_Ability3, ETriggerEvent::Completed, this, &ATitanPCCharacter::AbilityInputReleased, 2);
        }
        if (IA_Ability4) 
        {
            EIC->BindAction(IA_Ability4, ETriggerEvent::Started, this, &ATitanPCCharacter::AbilityInputPressed, 3);
            EIC->BindAction(IA_Ability4, ETriggerEvent::Completed, this, &ATitanPCCharacter::AbilityInputReleased, 3);
        }
    }
}

// Movement Functions
void ATitanPCCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Controller && (Axis.SizeSquared() > 0.f))
    {
        const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
        const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Forward, Axis.Y);
        AddMovementInput(Right, Axis.X);
    }
}

void ATitanPCCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y);
}

void ATitanPCCharacter::JumpPressed()
{
    if (EnhancedMovementComponent && EnhancedMovementComponent->IsWallRunning())
    {
        EnhancedMovementComponent->StopWallRun();
    }
    else
    {
        Jump();
    }
}

void ATitanPCCharacter::JumpReleased()
{
    StopJumping();
}

void ATitanPCCharacter::SprintPressed()
{
    if (EnhancedMovementComponent)
    {
        bIsSprinting = true;
        EnhancedMovementComponent->StartSprinting();
    }
}

void ATitanPCCharacter::SprintReleased()
{
    if (EnhancedMovementComponent)
    {
        bIsSprinting = false;
        EnhancedMovementComponent->StopSprinting();
    }
}

void ATitanPCCharacter::CrouchPressed()
{
    if (CanCrouch())
    {
        Crouch();
    }
    else
    {
        UnCrouch();
    }
}

void ATitanPCCharacter::SlidePressed()
{
    if (EnhancedMovementComponent)
    {
        EnhancedMovementComponent->StartSliding();
    }
}

void ATitanPCCharacter::WallRunPressed()
{
    if (EnhancedMovementComponent)
    {
        EnhancedMovementComponent->TryStartWallRun();
    }
}

void ATitanPCCharacter::DashPressed()
{
    if (EnhancedMovementComponent && bCanDash)
    {
        FVector DashDirection = GetLastMovementInputVector();
        if (DashDirection.IsZero())
        {
            DashDirection = GetActorForwardVector();
        }
        
        if (EnhancedMovementComponent->TryDash(DashDirection))
        {
            bCanDash = false;
            GetWorld()->GetTimerManager().SetTimer(DashCooldownTimer, this, &ATitanPCCharacter::ResetDashCooldown, DashCooldownTime, false);
        }
    }
}

// Combat Functions
void ATitanPCCharacter::FirePressed()
{
    if (WeaponComponent)
    {
        WeaponComponent->StartFiring();
    }
}

void ATitanPCCharacter::FireReleased()
{
    if (WeaponComponent)
    {
        WeaponComponent->StopFiring();
    }
}

void ATitanPCCharacter::AimPressed()
{
    if (WeaponComponent)
    {
        bIsAiming = true;
        WeaponComponent->StartAiming();
        
        // Adjust camera for aiming
        if (SpringArm)
        {
            SpringArm->TargetArmLength = 250.0f; // Closer for aiming
        }
    }
}

void ATitanPCCharacter::AimReleased()
{
    if (WeaponComponent)
    {
        bIsAiming = false;
        WeaponComponent->StopAiming();
        
        // Reset camera
        if (SpringArm)
        {
            SpringArm->TargetArmLength = 350.0f; // Normal distance
        }
    }
}

void ATitanPCCharacter::ReloadPressed()
{
    if (WeaponComponent)
    {
        WeaponComponent->TryReload();
    }
}

void ATitanPCCharacter::MeleePressed()
{
    if (WeaponComponent)
    {
        WeaponComponent->PerformMeleeAttack();
    }
}

// Interaction Functions
void ATitanPCCharacter::InteractPressed()
{
    // TODO: Implement interaction with objects
    UE_LOG(LogTemp, Warning, TEXT("Interact pressed"));
}

void ATitanPCCharacter::InventoryPressed()
{
    // TODO: Open inventory UI
    UE_LOG(LogTemp, Warning, TEXT("Inventory pressed"));
}

// Ability Functions
void ATitanPCCharacter::AbilityInputPressed(int32 InputID)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AbilityLocalInputPressed(InputID);
    }
}

void ATitanPCCharacter::AbilityInputReleased(int32 InputID)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AbilityLocalInputReleased(InputID);
    }
}

void ATitanPCCharacter::InitializeAbilitySystem()
{
    if (!AbilitySystemComponent) return;

    // Grant startup abilities
    for (TSubclassOf<UGameplayAbility>& StartupAbility : StartupAbilities)
    {
        AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
    }

    // Apply startup effects
    for (TSubclassOf<UGameplayEffect>& GameplayEffect : StartupEffects)
    {
        FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
        EffectContext.AddSourceObject(this);

        FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
        if (NewHandle.IsValid())
        {
            FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
        }
    }

    // Apply startup tags
    AbilitySystemComponent->AddLooseGameplayTags(StartupTags);
}

void ATitanPCCharacter::GrantStartupAbilities()
{
    // Already handled in InitializeAbilitySystem
}

void ATitanPCCharacter::BindAbilityActivationToInputComponent()
{
    // Handled in SetupPlayerInputComponent
}

// Attribute Callbacks
void ATitanPCCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
    float Health = Data.NewValue;
    float MaxHealth = AttributeSet->GetMaxHealth();
    
    OnHealthChanged.Broadcast(Health, MaxHealth);
    
    if (Health <= 0.0f)
    {
        // Handle death
        UE_LOG(LogTemp, Warning, TEXT("Character died"));
    }
}

void ATitanPCCharacter::OnStaminaAttributeChanged(const FOnAttributeChangeData& Data)
{
    float Stamina = Data.NewValue;
    float MaxStamina = AttributeSet->GetMaxStamina();
    
    OnStaminaChanged.Broadcast(Stamina, MaxStamina);
}

void ATitanPCCharacter::OnLevelAttributeChanged(const FOnAttributeChangeData& Data)
{
    int32 NewLevel = static_cast<int32>(Data.NewValue);
    OnLevelChanged.Broadcast(NewLevel);
    
    UE_LOG(LogTemp, Warning, TEXT("Character leveled up to level %d"), NewLevel);
}

// Getter Functions
float ATitanPCCharacter::GetHealth() const
{
    return AttributeSet ? AttributeSet->GetHealth() : 0.0f;
}

float ATitanPCCharacter::GetMaxHealth() const
{
    return AttributeSet ? AttributeSet->GetMaxHealth() : 0.0f;
}

float ATitanPCCharacter::GetStamina() const
{
    return AttributeSet ? AttributeSet->GetStamina() : 0.0f;
}

float ATitanPCCharacter::GetMaxStamina() const
{
    return AttributeSet ? AttributeSet->GetMaxStamina() : 0.0f;
}

int32 ATitanPCCharacter::GetCharacterLevel() const
{
    return AttributeSet ? static_cast<int32>(AttributeSet->GetCharacterLevel()) : 1;
}

// Ability System Public Interface
bool ATitanPCCharacter::TryActivateAbilityByTag(FGameplayTag AbilityTag)
{
    return AbilitySystemComponent ? AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag)) : false;
}

void ATitanPCCharacter::CancelAbilityByTag(FGameplayTag AbilityTag)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAbilities(&FGameplayTagContainer(AbilityTag));
    }
}

void ATitanPCCharacter::ResetDashCooldown()
{
    bCanDash = true;
}
