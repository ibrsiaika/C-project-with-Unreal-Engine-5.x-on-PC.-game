#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TitanPCDropShipSequence.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class ATitanPCCharacter;

/**
 * Simple cinematic drop-ship sequence that flies over the map like a battle royale plane,
 * then ejects the player and hands control back.
 *
 * This is asset-light (no meshes required) and focuses on camera + movement so you can run it immediately.
 * You can later swap in a plane mesh and Niagara trails via Blueprint or defaults.
 */
UCLASS()
class TITANPC_API ATitanPCDropShipSequence : public AActor
{
    GENERATED_BODY()

public:
    ATitanPCDropShipSequence();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // Entry point to start the sequence. Pass owning controller and the pawn to re-possess at the end.
    UFUNCTION(BlueprintCallable, Category="DropShip")
    void StartSequence(class APlayerController* InController, ATitanPCCharacter* InCharacter);

    // Waypoints for flight path (world positions). If empty, a default straight path will be generated.
    UPROPERTY(EditAnywhere, Category="DropShip|Path")
    TArray<FVector> Waypoints;

    // Speed of the plane along the path
    UPROPERTY(EditAnywhere, Category="DropShip|Path")
    float FlySpeed = 2000.0f;

    // Height offset applied to waypoints to ensure the plane flies high
    UPROPERTY(EditAnywhere, Category="DropShip|Path")
    float Altitude = 3000.0f;

    // Time the sequence waits at final waypoint before eject
    UPROPERTY(EditAnywhere, Category="DropShip|Sequence")
    float HoldAtEndTime = 1.5f;

    // If true, will smoothly move the character from sky to ground after eject to simulate a quick drop
    UPROPERTY(EditAnywhere, Category="DropShip|Sequence")
    bool bSimulateFastDrop = true;

    // Drop speed when simulating fast drop
    UPROPERTY(EditAnywhere, Category="DropShip|Sequence")
    float DropSpeed = 2500.0f;

    // Optional: attach a simple static mesh to visualize the plane (you can set this in editor)
    UPROPERTY(EditAnywhere, Category="DropShip|Visual")
    UStaticMesh* PlaneStaticMeshAsset;

    // Camera used for the cinematic. PlayerController will view through this until sequence ends.
    UPROPERTY(VisibleAnywhere, Category="DropShip|Visual")
    UCameraComponent* SequenceCamera;

    // Fired when the sequence finishes and the player is re-possessed
    UPROPERTY(BlueprintAssignable, Category="DropShip|Events")
    FSimpleMulticastDelegate OnSequenceFinished;

protected:
    void AdvanceAlongPath(float DeltaSeconds);
    void EjectPlayer();
    void UpdateCameraTransform();

    // Generates a simple default path if none provided: left-to-right across the current map bounds.
    void BuildDefaultPath();

private:
    UPROPERTY()
    APlayerController* OwningController;

    UPROPERTY()
    ATitanPCCharacter* TargetCharacter;

    UPROPERTY()
    UStaticMeshComponent* PlaneMeshComponent;

    int32 CurrentWaypointIndex = 0;
    FVector CurrentLocation = FVector::ZeroVector;
    bool bSequenceActive = false;
    bool bDroppingPlayer = false;
    float EndHoldTimer = 0.0f;

    // For drop sim
    FVector DropStartLocation = FVector::ZeroVector;
    FVector DropTargetLocation = FVector::ZeroVector;
};