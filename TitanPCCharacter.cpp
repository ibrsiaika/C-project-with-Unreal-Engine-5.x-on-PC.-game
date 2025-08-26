
#include "TitanPCCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

ATitanPCCharacter::ATitanPCCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 350.f;
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
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
}

void ATitanPCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (IA_Move) EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATitanPCCharacter::Move);
        if (IA_Look) EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ATitanPCCharacter::Look);
        if (IA_Jump) EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &ATitanPCCharacter::JumpPressed);
        if (IA_Sprint) {
            EIC->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ATitanPCCharacter::SprintPressed);
            EIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ATitanPCCharacter::SprintReleased);
        }
    }
}

void ATitanPCCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Controller && (Axis.SizeSquared() > 0.f))
    {
        const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
        const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        const FVector Right   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
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
    Jump();
}

void ATitanPCCharacter::SprintPressed()
{
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ATitanPCCharacter::SprintReleased()
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
