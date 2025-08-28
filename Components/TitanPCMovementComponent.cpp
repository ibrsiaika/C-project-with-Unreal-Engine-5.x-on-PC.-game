#include "TitanPCMovementComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "../Components/TitanPCHealthComponent.h"

UTitanPCMovementComponent::UTitanPCMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Set default movement values
    MaxWalkSpeed = 500.0f;
    MaxAcceleration = 2048.0f;
    BrakingFrictionFactor = 2.0f;
    BrakingDecelerationWalking = 2048.0f;
    
    // Enable advanced movement features
    bCanWalkOffLedgesWhenCrouching = true;
    bMaintainHorizontalGroundVelocity = true;
    bImpartBaseVelocityX = true;
    bImpartBaseVelocityY = true;
    bImpartBaseVelocityZ = true;
}

void UTitanPCMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentMovementState = ETitanPCMovementState::Walking;
}

void UTitanPCMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    switch (CurrentMovementState)
    {
        case ETitanPCMovementState::Sliding:
            UpdateSliding(DeltaTime);
            break;
        case ETitanPCMovementState::WallRunning:
            UpdateWallRun(DeltaTime);
            break;
        case ETitanPCMovementState::Dashing:
            UpdateDashing(DeltaTime);
            break;
        case ETitanPCMovementState::Climbing:
            UpdateClimbing(DeltaTime);
            break;
        default:
            break;
    }
}

void UTitanPCMovementComponent::StartSprinting()
{
    if (CanDash() && ConsumeStamina(0)) // Check if we have stamina
    {
        MaxWalkSpeed = SprintSpeed;
        SetMovementState(ETitanPCMovementState::Sprinting);
    }
}

void UTitanPCMovementComponent::StopSprinting()
{
    if (CurrentMovementState == ETitanPCMovementState::Sprinting)
    {
        MaxWalkSpeed = 500.0f; // Default walk speed
        SetMovementState(ETitanPCMovementState::Walking);
    }
}

void UTitanPCMovementComponent::StartSliding()
{
    if (CanSlide() && ConsumeStamina(SlideStaminaCost))
    {
        // Store original capsule height
        if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
        {
            Character->GetCapsuleComponent()->SetCapsuleHalfHeight(SlideCrouchHeight);
            
            // Add initial slide velocity
            FVector SlideVelocity = Character->GetActorForwardVector() * SlideSpeed;
            SlideVelocity.Z = Velocity.Z; // Maintain vertical velocity
            Velocity = SlideVelocity;
            
            SlideTimer = 0.0f;
            SetMovementState(ETitanPCMovementState::Sliding);
        }
    }
}

void UTitanPCMovementComponent::StopSliding()
{
    if (CurrentMovementState == ETitanPCMovementState::Sliding)
    {
        if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
        {
            Character->GetCapsuleComponent()->SetCapsuleHalfHeight(Character->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetCapsuleHalfHeight());
        }
        
        SetMovementState(ETitanPCMovementState::Walking);
    }
}

bool UTitanPCMovementComponent::TryStartWallRun()
{
    if (!CanWallRun() || !ConsumeStamina(WallRunStaminaCost))
    {
        return false;
    }
    
    FVector WallNormal, WallLocation;
    if (FindWallRunSurface(WallNormal, WallLocation))
    {
        WallRunNormal = WallNormal;
        WallRunTimer = 0.0f;
        GravityScale = WallRunGravityScale;
        
        // Determine wall run direction (left or right)
        FVector CrossProduct = FVector::CrossProduct(GetOwner()->GetActorForwardVector(), WallNormal);
        bWallRunningLeft = CrossProduct.Z > 0;
        
        SetMovementState(ETitanPCMovementState::WallRunning);
        return true;
    }
    
    return false;
}

void UTitanPCMovementComponent::StopWallRun()
{
    if (CurrentMovementState == ETitanPCMovementState::WallRunning)
    {
        GravityScale = 1.0f;
        WallRunNormal = FVector::ZeroVector;
        SetMovementState(ETitanPCMovementState::Walking);
        
        // Add jump off wall
        if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
        {
            FVector JumpDirection = WallRunNormal + FVector::UpVector;
            JumpDirection.Normalize();
            Character->LaunchCharacter(JumpDirection * WallRunJumpForce, true, true);
        }
    }
}

bool UTitanPCMovementComponent::TryDash(FVector DashDirection)
{
    if (!CanDash() || !ConsumeStamina(DashStaminaCost))
    {
        return false;
    }
    
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastDashTime < DashCooldown)
    {
        return false;
    }
    
    // Normalize dash direction and apply dash
    DashDirection.Normalize();
    this->DashDirection = DashDirection;
    DashTimer = 0.0f;
    LastDashTime = CurrentTime;
    
    // Apply immediate dash velocity
    Velocity = DashDirection * DashSpeed;
    
    SetMovementState(ETitanPCMovementState::Dashing);
    return true;
}

bool UTitanPCMovementComponent::TryClimb()
{
    if (!ConsumeStamina(ClimbStaminaCost))
    {
        return false;
    }
    
    FVector ClimbNormal, ClimbLocation;
    if (FindClimbSurface(ClimbNormal, ClimbLocation))
    {
        this->ClimbNormal = ClimbNormal;
        GravityScale = 0.0f;
        SetMovementState(ETitanPCMovementState::Climbing);
        return true;
    }
    
    return false;
}

void UTitanPCMovementComponent::StopClimbing()
{
    if (CurrentMovementState == ETitanPCMovementState::Climbing)
    {
        GravityScale = 1.0f;
        ClimbNormal = FVector::ZeroVector;
        SetMovementState(ETitanPCMovementState::Walking);
    }
}

bool UTitanPCMovementComponent::CanWallRun() const
{
    return !IsMovingOnGround() && 
           CurrentMovementState != ETitanPCMovementState::WallRunning &&
           CurrentMovementState != ETitanPCMovementState::Dashing &&
           Velocity.Z > -500.0f; // Not falling too fast
}

bool UTitanPCMovementComponent::CanSlide() const
{
    return IsMovingOnGround() && 
           CurrentMovementState != ETitanPCMovementState::Sliding &&
           Velocity.Size() > 300.0f; // Must have some speed
}

bool UTitanPCMovementComponent::CanDash() const
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    return CurrentMovementState != ETitanPCMovementState::Dashing &&
           (CurrentTime - LastDashTime) >= DashCooldown;
}

void UTitanPCMovementComponent::SetStaminaComponent(UTitanPCHealthComponent* InStaminaComponent)
{
    StaminaComponent = InStaminaComponent;
}

void UTitanPCMovementComponent::UpdateWallRun(float DeltaTime)
{
    WallRunTimer += DeltaTime;
    
    // Consume stamina
    if (!ConsumeStamina(WallRunStaminaCost * DeltaTime))
    {
        StopWallRun();
        return;
    }
    
    // Check if we should stop wall running
    if (WallRunTimer > MaxWallRunTime || IsMovingOnGround())
    {
        StopWallRun();
        return;
    }
    
    // Verify we still have a wall to run on
    FVector WallNormal, WallLocation;
    if (!FindWallRunSurface(WallNormal, WallLocation))
    {
        StopWallRun();
        return;
    }
    
    // Apply wall run forces
    ApplyWallRunForce(WallNormal, DeltaTime);
}

void UTitanPCMovementComponent::UpdateSliding(float DeltaTime)
{
    SlideTimer += DeltaTime;
    
    if (ShouldStopSliding())
    {
        StopSliding();
        return;
    }
    
    ApplySlidingForce(DeltaTime);
}

void UTitanPCMovementComponent::UpdateDashing(float DeltaTime)
{
    DashTimer += DeltaTime;
    
    if (DashTimer >= DashDuration)
    {
        EndDash();
    }
    else
    {
        // Maintain dash velocity
        Velocity = DashDirection * DashSpeed;
    }
}

void UTitanPCMovementComponent::UpdateClimbing(float DeltaTime)
{
    // Consume stamina
    if (!ConsumeStamina(ClimbStaminaCost * DeltaTime))
    {
        StopClimbing();
        return;
    }
    
    // Check if we still have a surface to climb
    FVector ClimbNormal, ClimbLocation;
    if (!FindClimbSurface(ClimbNormal, ClimbLocation))
    {
        StopClimbing();
        return;
    }
    
    ApplyClimbingForce(ClimbNormal, DeltaTime);
}

bool UTitanPCMovementComponent::FindWallRunSurface(FVector& OutWallNormal, FVector& OutWallLocation)
{
    if (!GetOwner()) return false;
    
    // Cast rays to the left and right to find walls
    FVector StartLocation = GetOwner()->GetActorLocation();
    TArray<FVector> TraceDirections = {
        GetOwner()->GetActorRightVector(),
        -GetOwner()->GetActorRightVector()
    };
    
    for (const FVector& Direction : TraceDirections)
    {
        FVector EndLocation = StartLocation + Direction * WallRunTraceDistance;
        
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(GetOwner());
        
        if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, WallRunTraceChannel, QueryParams))
        {
            if (IsValidWallRunSurface(HitResult))
            {
                OutWallNormal = HitResult.Normal;
                OutWallLocation = HitResult.Location;
                return true;
            }
        }
    }
    
    return false;
}

bool UTitanPCMovementComponent::FindClimbSurface(FVector& OutClimbNormal, FVector& OutClimbLocation)
{
    if (!GetOwner()) return false;
    
    FVector StartLocation = GetOwner()->GetActorLocation();
    FVector EndLocation = StartLocation + GetOwner()->GetActorForwardVector() * ClimbTraceDistance;
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ClimbTraceChannel, QueryParams))
    {
        float WallAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.Normal, FVector::UpVector)));
        if (WallAngle > 70.0f) // Wall is steep enough to climb
        {
            OutClimbNormal = HitResult.Normal;
            OutClimbLocation = HitResult.Location;
            return true;
        }
    }
    
    return false;
}

void UTitanPCMovementComponent::ApplyWallRunForce(FVector WallNormal, float DeltaTime)
{
    // Calculate wall run direction (perpendicular to wall normal)
    FVector WallRunDirection = FVector::CrossProduct(WallNormal, FVector::UpVector);
    if (!bWallRunningLeft)
    {
        WallRunDirection *= -1;
    }
    
    // Apply forward movement along the wall
    FVector TargetVelocity = WallRunDirection * WallRunSpeed;
    TargetVelocity.Z = FMath::Max(Velocity.Z, -200.0f); // Limit falling speed
    
    Velocity = FMath::VInterpTo(Velocity, TargetVelocity, DeltaTime, 5.0f);
}

void UTitanPCMovementComponent::ApplySlidingForce(float DeltaTime)
{
    // Apply deceleration to sliding
    float CurrentSpeed = Velocity.Size2D();
    float NewSpeed = FMath::Max(CurrentSpeed - SlideDeceleration * DeltaTime, SlideMinSpeed);
    
    FVector SlideDirection = Velocity;
    SlideDirection.Z = 0;
    SlideDirection.Normalize();
    
    Velocity = SlideDirection * NewSpeed;
    Velocity.Z = FMath::Min(Velocity.Z, 0.0f); // Don't gain upward velocity while sliding
}

void UTitanPCMovementComponent::ApplyClimbingForce(FVector ClimbNormal, float DeltaTime)
{
    // Get input direction for climbing
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        FVector InputDirection = Character->GetLastMovementInputVector();
        
        // Project input onto the climbing surface
        FVector ClimbDirection = InputDirection - FVector::DotProduct(InputDirection, ClimbNormal) * ClimbNormal;
        ClimbDirection.Normalize();
        
        Velocity = ClimbDirection * ClimbSpeed;
    }
}

bool UTitanPCMovementComponent::IsValidWallRunSurface(const FHitResult& Hit)
{
    // Check if the surface is steep enough for wall running
    float WallAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Hit.Normal, FVector::UpVector)));
    return WallAngle >= WallRunMinAngle;
}

bool UTitanPCMovementComponent::ShouldStopSliding()
{
    return SlideTimer > MaxSlideTime || 
           Velocity.Size2D() < SlideMinSpeed || 
           !IsMovingOnGround();
}

void UTitanPCMovementComponent::EndDash()
{
    SetMovementState(IsMovingOnGround() ? ETitanPCMovementState::Walking : ETitanPCMovementState::None);
}

void UTitanPCMovementComponent::SetMovementState(ETitanPCMovementState NewState)
{
    if (CurrentMovementState != NewState)
    {
        PreviousMovementState = CurrentMovementState;
        CurrentMovementState = NewState;
        OnMovementStateChanged.Broadcast(CurrentMovementState);
    }
}

bool UTitanPCMovementComponent::ConsumeStamina(float StaminaCost)
{
    if (!StaminaComponent || StaminaCost <= 0.0f)
    {
        return true; // No stamina system or no cost
    }
    
    float CurrentStamina = StaminaComponent->GetStamina();
    if (CurrentStamina < MinimumStamina && StaminaCost > 0)
    {
        return false; // Not enough stamina
    }
    
    // For continuous costs (per second), we just check if we have enough
    // The actual consumption would be handled by the stamina component
    return CurrentStamina >= StaminaCost;
}