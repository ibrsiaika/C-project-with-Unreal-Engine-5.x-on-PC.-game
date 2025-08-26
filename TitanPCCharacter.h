
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TitanPCCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class TITANPC_API ATitanPCCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATitanPCCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Movement
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void JumpPressed();
    void SprintPressed();
    void SprintReleased();

private:
    UPROPERTY(VisibleAnywhere, Category="Camera")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, Category="Camera")
    UCameraComponent* Camera;

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

    UPROPERTY(EditAnywhere, Category="Movement")
    float WalkSpeed = 500.f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float SprintSpeed = 800.f;
};
