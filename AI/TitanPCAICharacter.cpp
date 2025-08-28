#include "TitanPCAICharacter.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/TitanPCHealthComponent.h"
#include "../Components/TitanPCWeaponComponent.h"
#include "../GAS/TitanPCAttributeSet.h"
#include "Engine/Engine.h"

ATitanPCAICharacter::ATitanPCAICharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create Ability System Component
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Create Attribute Set
    AttributeSet = CreateDefaultSubobject<UTitanPCAttributeSet>(TEXT("AttributeSet"));

    // Create Health Component
    HealthComponent = CreateDefaultSubobject<UTitanPCHealthComponent>(TEXT("HealthComponent"));

    // Create Weapon Component
    WeaponComponent = CreateDefaultSubobject<UTitanPCWeaponComponent>(TEXT("WeaponComponent"));

    // Create AI Perception Component
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    // Setup movement
    GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;

    // Setup collision
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

    // Set replication
    bReplicates = true;
    SetReplicateMovement(true);
}

UAbilitySystemComponent* ATitanPCAICharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ATitanPCAICharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }

    SetupBehaviorTree();
}

void ATitanPCAICharacter::BeginPlay()
{
    Super::BeginPlay();

    HomeLocation = GetActorLocation();
    InitializeAIComponents();

    // Bind health events
    if (HealthComponent)
    {
        HealthComponent->OnHealthChanged.AddDynamic(this, &ATitanPCAICharacter::HandleHealthChanged);
        HealthComponent->OnDeath.AddDynamic(this, &ATitanPCAICharacter::HandleDeath);
    }

    UE_LOG(LogTemp, Warning, TEXT("AI Character %s initialized"), *GetName());
}

void ATitanPCAICharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update AI state based on current conditions
    switch (CurrentAIState)
    {
        case ETitanPCAIState::Combat:
            if (CombatTarget && !IsValid(CombatTarget))
            {
                StopCombat();
            }
            break;
        case ETitanPCAIState::Chasing:
            if (CombatTarget && FVector::Dist(GetActorLocation(), CombatTarget->GetActorLocation()) <= CombatRange)
            {
                StartCombat(CombatTarget);
            }
            break;
        default:
            break;
    }
}

void ATitanPCAICharacter::SetAIState(ETitanPCAIState NewState)
{
    if (CurrentAIState != NewState)
    {
        PreviousAIState = CurrentAIState;
        CurrentAIState = NewState;

        // Update blackboard
        if (AAIController* AIController = Cast<AAIController>(GetController()))
        {
            if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
            {
                BlackboardComp->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(CurrentAIState));
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("AI %s changed state from %d to %d"), 
            *GetName(), static_cast<int32>(PreviousAIState), static_cast<int32>(CurrentAIState));
    }
}

void ATitanPCAICharacter::StartCombat(AActor* Target)
{
    if (!Target) return;

    CombatTarget = Target;
    SetAIState(ETitanPCAIState::Combat);

    // Update blackboard
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
        {
            BlackboardComp->SetValueAsObject(TEXT("CombatTarget"), CombatTarget);
        }
    }

    // Start attacking if we have a weapon
    if (WeaponComponent)
    {
        WeaponComponent->StartFiring();
    }
}

void ATitanPCAICharacter::StopCombat()
{
    CombatTarget = nullptr;
    SetAIState(ETitanPCAIState::Patrolling);

    // Update blackboard
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
        {
            BlackboardComp->ClearValue(TEXT("CombatTarget"));
        }
    }

    // Stop attacking
    if (WeaponComponent)
    {
        WeaponComponent->StopFiring();
    }
}

bool ATitanPCAICharacter::CanSeeActor(AActor* Actor) const
{
    if (!Actor || !AIPerceptionComponent) return false;

    FActorPerceptionBlueprintInfo Info;
    AIPerceptionComponent->GetActorsPerception(Actor, Info);
    
    return Info.LastSensedStimuli.Num() > 0 && Info.LastSensedStimuli[0].WasSuccessfullySensed();
}

void ATitanPCAICharacter::InitializeAIComponents()
{
    if (AIPerceptionComponent)
    {
        // Configure Sight Sense
        UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
        if (SightConfig)
        {
            SightConfig->SightRadius = SightRange;
            SightConfig->LoseSightRadius = SightRange * 1.2f;
            SightConfig->PeripheralVisionAngleDegrees = SightAngle;
            SightConfig->SetMaxAge(3.0f);
            SightConfig->DetectionByAffiliation.bNeutral = true;
            SightConfig->DetectionByAffiliation.bFriendly = false;
            SightConfig->DetectionByAffiliation.bEnemy = true;
            
            AIPerceptionComponent->ConfigureSense(*SightConfig);
        }

        // Configure Hearing Sense
        UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
        if (HearingConfig)
        {
            HearingConfig->HearingRange = HearingRange;
            HearingConfig->SetMaxAge(3.0f);
            HearingConfig->DetectionByAffiliation.bNeutral = true;
            HearingConfig->DetectionByAffiliation.bFriendly = false;
            HearingConfig->DetectionByAffiliation.bEnemy = true;

            AIPerceptionComponent->ConfigureSense(*HearingConfig);
        }

        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    }
}

void ATitanPCAICharacter::SetupBehaviorTree()
{
    if (BehaviorTree)
    {
        if (AAIController* AIController = Cast<AAIController>(GetController()))
        {
            AIController->RunBehaviorTree(BehaviorTree);

            // Initialize blackboard values
            if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
            {
                BlackboardComp->SetValueAsVector(TEXT("HomeLocation"), HomeLocation);
                BlackboardComp->SetValueAsFloat(TEXT("PatrolRadius"), PatrolRadius);
                BlackboardComp->SetValueAsFloat(TEXT("CombatRange"), CombatRange);
                BlackboardComp->SetValueAsFloat(TEXT("MeleeRange"), MeleeRange);
                BlackboardComp->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(CurrentAIState));
            }
        }
    }
}

void ATitanPCAICharacter::HandleHealthChanged(float Health, float MaxHealth, float HealthPercentage)
{
    OnHealthChanged(Health, MaxHealth);

    // AI behavior based on health
    if (HealthPercentage < 0.3f && Courage < 0.5f)
    {
        // Low health and low courage - consider fleeing
        if (CurrentAIState == ETitanPCAIState::Combat)
        {
            SetAIState(ETitanPCAIState::Fleeing);
        }
    }
    else if (HealthPercentage > 0.8f && CurrentAIState == ETitanPCAIState::Fleeing)
    {
        // High health again - stop fleeing
        SetAIState(ETitanPCAIState::Patrolling);
    }
}

void ATitanPCAICharacter::HandleDeath()
{
    SetAIState(ETitanPCAIState::Dead);
    StopCombat();

    // Stop AI
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AIController->BrainComponent->StopLogic(TEXT("Death"));
    }

    // Disable collision and movement
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->DisableMovement();

    OnDeath();

    UE_LOG(LogTemp, Warning, TEXT("AI Character %s has died"), *GetName());
}