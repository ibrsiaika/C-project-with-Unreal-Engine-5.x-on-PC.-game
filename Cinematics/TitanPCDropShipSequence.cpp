#include "TitanPCDropShipSequence.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "TitanPCCharacter.h"

ATitanPCDropShipSequence::ATitanPCDropShipSequence()
{
    PrimaryActorTick.bCanEverTick = true;
    SetActorEnableCollision(false);
    bReplicates = false; // Local cinematic

    PlaneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
    PlaneMeshComponent->SetupAttachment(RootComponent);
    PlaneMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PlaneMeshComponent->SetVisibility(false); // Hidden until asset assigned

    SequenceCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SequenceCamera"));
    SequenceCamera->SetupAttachment(PlaneMeshComponent);
    SequenceCamera->bUsePawnControlRotation = false;
    SequenceCamera->SetFieldOfView(90.0f);
}

void ATitanPCDropShipSequence::BeginPlay()
{
    Super::BeginPlay();

    if (PlaneStaticMeshAsset)
    {
        PlaneMeshComponent->SetStaticMesh(PlaneStaticMeshAsset);
        PlaneMeshComponent->SetVisibility(true);
    }

    // Build a default path if none was provided
    if (Waypoints.Num() == 0)
    {
        BuildDefaultPath();
    }

    // Initialize starting location
    if (Waypoints.Num() > 0)
    {
        CurrentLocation = Waypoints[0] + FVector(0, 0, Altitude);
        SetActorLocation(CurrentLocation);
    }
}

void ATitanPCDropShipSequence::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bSequenceActive)
    {
        return;
    }

    if (!bDroppingPlayer)
    {
        AdvanceAlongPath(DeltaSeconds);

        // If we've reached final waypoint, hold briefly then eject
        if (CurrentWaypointIndex >= Waypoints.Num())
        {
            EndHoldTimer += DeltaSeconds;
            if (EndHoldTimer >= HoldAtEndTime)
            {
                EjectPlayer();
            }
        }
    }
    else
    {
        // Simulate fast drop: move character from sky to ground smoothly, then finish
        if (bSimulateFastDrop && TargetCharacter)
        {
            FVector NewLoc = TargetCharacter->GetActorLocation();
            FVector Direction = (DropTargetLocation - NewLoc).GetSafeNormal();
            float Dist = FVector::Dist(NewLoc, DropTargetLocation);
            float Step = DropSpeed * DeltaSeconds;

            if (Dist <= Step)
            {
                TargetCharacter->SetActorLocation(DropTargetLocation);
                bDroppingPlayer = false;
                OnSequenceFinished.Broadcast();

                // Return view to character and enable input
                if (OwningController && TargetCharacter)
                {
                    OwningController->SetViewTarget(TargetCharacter);
                    OwningController->Possess(TargetCharacter);
                    OwningController->bShowMouseCursor = false;
                    FInputModeGameOnly InputMode;
                    OwningController->SetInputMode(InputMode);
                }

                // Destroy sequence actor
                Destroy();
            }
            else
            {
                TargetCharacter->SetActorLocation(NewLoc + Direction * Step);
            }
        }
    }

    UpdateCameraTransform();
}

void ATitanPCDropShipSequence::StartSequence(APlayerController* InController, ATitanPCCharacter* InCharacter)
{
    OwningController = InController;
    TargetCharacter = InCharacter;

    if (!OwningController || !TargetCharacter || Waypoints.Num() == 0)
    {
        // Fail-safe: do nothing if missing critical info
        return;
    }

    bSequenceActive = true;
    CurrentWaypointIndex = 1; // Start moving toward waypoint 1 from 0

    // Put player into cinematic view and temporarily unpossess character
    OwningController->SetViewTarget(this);
    OwningController->UnPossess();
    OwningController->bShowMouseCursor = false;
    FInputModeGameOnly InputMode;
    OwningController->SetInputMode(InputMode);

    // Place character high above first waypoint so the drop feels coherent
    DropStartLocation = Waypoints[0] + FVector(0, 0, Altitude + 1000.0f);
    TargetCharacter->SetActorLocation(DropStartLocation);
    TargetCharacter->DisableInput(OwningController);
}

void ATitanPCDropShipSequence::AdvanceAlongPath(float DeltaSeconds)
{
    if (Waypoints.Num() == 0 || CurrentWaypointIndex >= Waypoints.Num())
    {
        return;
    }

    const FVector Target = Waypoints[CurrentWaypointIndex] + FVector(0, 0, Altitude);
    FVector Direction = (Target - CurrentLocation).GetSafeNormal();
    float Distance = FVector::Dist(CurrentLocation, Target);
    float Step = FlySpeed * DeltaSeconds;

    if (Distance <= Step)
    {
        CurrentLocation = Target;
        SetActorLocation(CurrentLocation);
        CurrentWaypointIndex++;
    }
    else
    {
        CurrentLocation += Direction * Step;
        SetActorLocation(CurrentLocation);
        SetActorRotation(Direction.Rotation());
    }
}

void ATitanPCDropShipSequence::EjectPlayer()
{
    bDroppingPlayer = true;

    // Trace down to find ground location near the final waypoint
    FVector Start = CurrentLocation;
    FVector End = Start - FVector(0, 0, 100000.0f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.bTraceComplex = false;
    Params.AddIgnoredActor(this);
    if (TargetCharacter)
    {
        Params.AddIgnoredActor(TargetCharacter);
    }

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
    {
        DropTargetLocation = Hit.Location + FVector(0, 0, 100.0f); // small offset above ground
    }
    else
    {
        // Fallback: just drop near last waypoint at low altitude
        DropTargetLocation = Waypoints.Last() + FVector(0, 0, 200.0f);
    }

    // Enable input again during drop so player can look around (optional)
    if (OwningController && TargetCharacter)
    {
        TargetCharacter->EnableInput(OwningController);
    }
}

void ATitanPCDropShipSequence::UpdateCameraTransform()
{
    // Slight nose-down tilt for cinematic feel
    FRotator CamRot = GetActorRotation();
    CamRot.Pitch = CamRot.Pitch - 5.0f;
    SequenceCamera->SetWorldLocation(GetActorLocation() + GetActorForwardVector() * 200.0f + FVector(0, 0, 100.0f));
    SequenceCamera->SetWorldRotation(CamRot);
}

void ATitanPCDropShipSequence::BuildDefaultPath()
{
    // Build a simple two-point path across the map centered around player start
    FVector Center = FVector::ZeroVector;

    // Try to find PlayerStart actor
    TArray<AActor*> FoundStarts;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("PlayerStart")), FoundStarts);
    if (FoundStarts.Num() > 0)
    {
        Center = FoundStarts[0]->GetActorLocation();
    }
    else
    {
        // Fallback: center of current level bounds via world origin
        Center = FVector::ZeroVector;
    }

    // Create a left-to-right flight path
    const float PathHalfLength = 50000.0f;
    FVector Left = Center + FVector(0, -PathHalfLength, 0);
    FVector Right = Center + FVector(0, PathHalfLength, 0);

    Waypoints.Add(Left);
    Waypoints.Add(Right);
}