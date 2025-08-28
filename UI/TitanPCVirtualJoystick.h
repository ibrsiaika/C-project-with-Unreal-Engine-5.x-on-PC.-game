#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Input/Reply.h"
#include "TitanPCVirtualJoystick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVirtualJoystickInput, FVector2D, InputValue);

UCLASS()
class TITANPC_API UTitanPCVirtualJoystick : public UWidget
{
    GENERATED_BODY()

public:
    UTitanPCVirtualJoystick();

    // Widget interface
    virtual void SynchronizeProperties() override;
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;

    // Input event
    UPROPERTY(BlueprintAssignable, Category = "Virtual Joystick")
    FOnVirtualJoystickInput OnInputChanged;

    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    FSlateBrush BackgroundImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    FSlateBrush ThumbImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    float InputScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bSnapToCenter = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    float DeadZone = 0.1f;

    UFUNCTION(BlueprintCallable, Category = "Virtual Joystick")
    FVector2D GetInputValue() const { return CurrentInput; }

    UFUNCTION(BlueprintCallable, Category = "Virtual Joystick")
    void ResetJoystick();

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    FVector2D CurrentInput = FVector2D::ZeroVector;
    TSharedPtr<class STitanPCVirtualJoystick> MyVirtualJoystick;

    void HandleInputChanged(FVector2D NewInput);
};

// Slate widget implementation
class STitanPCVirtualJoystick : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(STitanPCVirtualJoystick)
        : _BackgroundImage(nullptr)
        , _ThumbImage(nullptr)
        , _InputScale(1.0f)
        , _SnapToCenter(true)
        , _DeadZone(0.1f)
    {}

    SLATE_ARGUMENT(const FSlateBrush*, BackgroundImage)
    SLATE_ARGUMENT(const FSlateBrush*, ThumbImage)
    SLATE_ARGUMENT(float, InputScale)
    SLATE_ARGUMENT(bool, SnapToCenter)
    SLATE_ARGUMENT(float, DeadZone)
    SLATE_EVENT(FOnVirtualJoystickInput, OnInputChanged)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // SWidget interface
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FVector2D ComputeDesiredSize(float) const override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
    virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
    virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;

    void ResetInput();

private:
    const FSlateBrush* BackgroundImage;
    const FSlateBrush* ThumbImage;
    float InputScale;
    bool bSnapToCenter;
    float DeadZone;
    FOnVirtualJoystickInput OnInputChanged;

    FVector2D CurrentInput = FVector2D::ZeroVector;
    FVector2D CenterPosition = FVector2D::ZeroVector;
    bool bIsPressed = false;

    FVector2D CalculateInputFromPosition(const FGeometry& Geometry, const FVector2D& LocalPosition);
    void UpdateInput(const FVector2D& NewInput);
};