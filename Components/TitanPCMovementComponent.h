#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "TitanPCMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ETitanPCMovementState : uint8
{
    None            UMETA(DisplayName = "None"),
    Walking         UMETA(DisplayName = "Walking"),
    Sprinting       UMETA(DisplayName = "Sprinting"),
    Crouching       UMETA(DisplayName = "Crouching"),
    Sliding         UMETA(DisplayName = "Sliding"),
    WallRunning     UMETA(DisplayName = "Wall Running"),
    Dashing         UMETA(DisplayName = "Dashing"),
    Climbing        UMETA(DisplayName = "Climbing")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementStateChanged, ETitanPCMovementState, NewState);

UCLASS()
class TITANPC_API UTitanPCMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UTitanPCMovementComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Enhanced Movement Functions
    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    void StartSprinting();

    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    void StopSprinting();

    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    void StartSliding();

    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    void StopSliding();

    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    bool TryStartWallRun();

    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    void StopWallRun();

    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    bool TryDash(FVector DashDirection);

    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    bool TryClimb();

    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    void StopClimbing();

    // State Queries
    UFUNCTION(BlueprintPure, Category = "Enhanced Movement")
    ETitanPCMovementState GetMovementState() const { return CurrentMovementState; }

    UFUNCTION(BlueprintPure, Category = "Enhanced Movement")
    bool IsSliding() const { return CurrentMovementState == ETitanPCMovementState::Sliding; }

    UFUNCTION(BlueprintPure, Category = "Enhanced Movement")
    bool IsWallRunning() const { return CurrentMovementState == ETitanPCMovementState::WallRunning; }

    UFUNCTION(BlueprintPure, Category = "Enhanced Movement")
    bool IsDashing() const { return CurrentMovementState == ETitanPCMovementState::Dashing; }

    UFUNCTION(BlueprintPure, Category = "Enhanced Movement")
    bool IsClimbing() const { return CurrentMovementState == ETitanPCMovementState::Climbing; }

    UFUNCTION(BlueprintPure, Category = "Enhanced Movement")
    bool CanWallRun() const;

    UFUNCTION(BlueprintPure, Category = "Enhanced Movement")
    bool CanSlide() const;

    UFUNCTION(BlueprintPure, Category = "Enhanced Movement")
    bool CanDash() const;

    // Stamina Integration
    UFUNCTION(BlueprintCallable, Category = "Enhanced Movement")
    void SetStaminaComponent(class UTitanPCHealthComponent* InStaminaComponent);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Enhanced Movement")
    FOnMovementStateChanged OnMovementStateChanged;

protected:
    // Wall Running
    void UpdateWallRun(float DeltaTime);
    bool FindWallRunSurface(FVector& OutWallNormal, FVector& OutWallLocation);
    void ApplyWallRunForce(FVector WallNormal, float DeltaTime);
    bool IsValidWallRunSurface(const FHitResult& Hit);

    // Sliding
    void UpdateSliding(float DeltaTime);
    void ApplySlidingForce(float DeltaTime);
    bool ShouldStopSliding();

    // Dashing
    void UpdateDashing(float DeltaTime);
    void EndDash();

    // Climbing
    void UpdateClimbing(float DeltaTime);
    bool FindClimbSurface(FVector& OutClimbNormal, FVector& OutClimbLocation);
    void ApplyClimbingForce(FVector ClimbNormal, float DeltaTime);

    // State Management
    void SetMovementState(ETitanPCMovementState NewState);
    bool ConsumeStamina(float StaminaCost);

private:
    // Movement State
    ETitanPCMovementState CurrentMovementState = ETitanPCMovementState::None;
    ETitanPCMovementState PreviousMovementState = ETitanPCMovementState::None;

    // Stamina Component Reference
    UPROPERTY()
    class UTitanPCHealthComponent* StaminaComponent;

    // Sprinting
    UPROPERTY(EditDefaultsOnly, Category = "Sprint")
    float SprintSpeed = 800.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Sprint")
    float SprintStaminaCost = 10.0f; // Per second

    // Sliding
    UPROPERTY(EditDefaultsOnly, Category = "Sliding")
    float SlideSpeed = 1200.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Sliding")
    float SlideDeceleration = 1500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Sliding")
    float SlideMinSpeed = 300.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Sliding")
    float SlideStaminaCost = 15.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Sliding")
    float SlideCrouchHeight = 60.0f;

    float SlideTimer = 0.0f;
    float MaxSlideTime = 3.0f;

    // Wall Running
    UPROPERTY(EditDefaultsOnly, Category = "Wall Running")
    float WallRunSpeed = 700.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Wall Running")
    float WallRunStaminaCost = 20.0f; // Per second

    UPROPERTY(EditDefaultsOnly, Category = "Wall Running")
    float WallRunJumpForce = 800.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Wall Running")
    float WallRunTraceDistance = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Wall Running")
    float WallRunMinAngle = 75.0f; // Minimum angle for valid wall

    UPROPERTY(EditDefaultsOnly, Category = "Wall Running")
    float WallRunGravityScale = 0.3f;

    FVector WallRunNormal = FVector::ZeroVector;
    float WallRunTimer = 0.0f;
    float MaxWallRunTime = 5.0f;
    bool bWallRunningLeft = false;

    // Dashing
    UPROPERTY(EditDefaultsOnly, Category = "Dashing")
    float DashDistance = 1000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Dashing")
    float DashSpeed = 2000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Dashing")
    float DashDuration = 0.3f;

    UPROPERTY(EditDefaultsOnly, Category = "Dashing")
    float DashStaminaCost = 25.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Dashing")
    float DashCooldown = 2.0f;

    FVector DashDirection = FVector::ZeroVector;
    float DashTimer = 0.0f;
    float LastDashTime = 0.0f;

    // Climbing
    UPROPERTY(EditDefaultsOnly, Category = "Climbing")
    float ClimbSpeed = 300.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Climbing")
    float ClimbStaminaCost = 15.0f; // Per second

    UPROPERTY(EditDefaultsOnly, Category = "Climbing")
    float ClimbTraceDistance = 75.0f;

    FVector ClimbNormal = FVector::ZeroVector;

    // General Settings
    UPROPERTY(EditDefaultsOnly, Category = "General")
    float MinimumStamina = 10.0f; // Minimum stamina required for enhanced movement

    // Trace Channels
    UPROPERTY(EditDefaultsOnly, Category = "Collision")
    TEnumAsByte<ECollisionChannel> WallRunTraceChannel = ECC_WorldStatic;

    UPROPERTY(EditDefaultsOnly, Category = "Collision")
    TEnumAsByte<ECollisionChannel> ClimbTraceChannel = ECC_WorldStatic;
};