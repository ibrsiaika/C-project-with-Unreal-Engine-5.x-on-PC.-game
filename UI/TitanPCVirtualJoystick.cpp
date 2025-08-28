#include "TitanPCVirtualJoystick.h"
#include "Rendering/DrawElements.h"
#include "Framework/Application/SlateApplication.h"

// UMG Widget Implementation
UTitanPCVirtualJoystick::UTitanPCVirtualJoystick()
{
    bIsVariable = false;
    SetVisibility(ESlateVisibility::Visible);
    
    // Set default brush properties
    BackgroundImage.DrawAs = ESlateBrushDrawType::Image;
    BackgroundImage.Tiling = ESlateBrushTileType::NoTile;
    
    ThumbImage.DrawAs = ESlateBrushDrawType::Image;
    ThumbImage.Tiling = ESlateBrushTileType::NoTile;
}

void UTitanPCVirtualJoystick::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    
    if (MyVirtualJoystick.IsValid())
    {
        // Update properties in Slate widget
        // This would normally update the Slate widget's properties
    }
}

void UTitanPCVirtualJoystick::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MyVirtualJoystick.Reset();
}

void UTitanPCVirtualJoystick::ResetJoystick()
{
    CurrentInput = FVector2D::ZeroVector;
    if (MyVirtualJoystick.IsValid())
    {
        MyVirtualJoystick->ResetInput();
    }
}

TSharedRef<SWidget> UTitanPCVirtualJoystick::RebuildWidget()
{
    MyVirtualJoystick = SNew(STitanPCVirtualJoystick)
        .BackgroundImage(&BackgroundImage)
        .ThumbImage(&ThumbImage)
        .InputScale(InputScale)
        .SnapToCenter(bSnapToCenter)
        .DeadZone(DeadZone)
        .OnInputChanged(BIND_UOBJECT_DELEGATE(FOnVirtualJoystickInput, HandleInputChanged));

    return MyVirtualJoystick.ToSharedRef();
}

void UTitanPCVirtualJoystick::HandleInputChanged(FVector2D NewInput)
{
    CurrentInput = NewInput;
    OnInputChanged.Broadcast(CurrentInput);
}

// Slate Widget Implementation
void STitanPCVirtualJoystick::Construct(const FArguments& InArgs)
{
    BackgroundImage = InArgs._BackgroundImage;
    ThumbImage = InArgs._ThumbImage;
    InputScale = InArgs._InputScale;
    bSnapToCenter = InArgs._SnapToCenter;
    DeadZone = InArgs._DeadZone;
    OnInputChanged = InArgs._OnInputChanged;
}

int32 STitanPCVirtualJoystick::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    int32 RetLayerId = LayerId;
    
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const FVector2D Center = LocalSize * 0.5f;
    
    // Draw background
    if (BackgroundImage && BackgroundImage->GetResourceObject())
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            RetLayerId++,
            AllottedGeometry.ToPaintGeometry(),
            BackgroundImage,
            ESlateDrawEffect::None,
            InWidgetStyle.GetColorAndOpacityTint()
        );
    }
    
    // Calculate thumb position
    FVector2D ThumbPosition = Center + (CurrentInput * LocalSize * 0.4f); // 0.4f is max radius
    FVector2D ThumbSize = LocalSize * 0.3f; // Thumb is 30% of widget size
    
    // Draw thumb
    if (ThumbImage && ThumbImage->GetResourceObject())
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            RetLayerId++,
            AllottedGeometry.ToPaintGeometry(ThumbSize, FSlateLayoutTransform(ThumbPosition - ThumbSize * 0.5f)),
            ThumbImage,
            ESlateDrawEffect::None,
            InWidgetStyle.GetColorAndOpacityTint()
        );
    }
    
    return RetLayerId;
}

FVector2D STitanPCVirtualJoystick::ComputeDesiredSize(float) const
{
    return FVector2D(120.0f, 120.0f); // Default size
}

FReply STitanPCVirtualJoystick::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsPressed = true;
        CenterPosition = MyGeometry.GetLocalSize() * 0.5f;
        
        FVector2D LocalMousePos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
        FVector2D NewInput = CalculateInputFromPosition(MyGeometry, LocalMousePos);
        UpdateInput(NewInput);
        
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }
    
    return FReply::Unhandled();
}

FReply STitanPCVirtualJoystick::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsPressed)
    {
        bIsPressed = false;
        
        if (bSnapToCenter)
        {
            UpdateInput(FVector2D::ZeroVector);
        }
        
        return FReply::Handled().ReleaseMouseCapture();
    }
    
    return FReply::Unhandled();
}

FReply STitanPCVirtualJoystick::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (bIsPressed)
    {
        FVector2D LocalMousePos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
        FVector2D NewInput = CalculateInputFromPosition(MyGeometry, LocalMousePos);
        UpdateInput(NewInput);
        
        return FReply::Handled();
    }
    
    return FReply::Unhandled();
}

FReply STitanPCVirtualJoystick::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
    bIsPressed = true;
    CenterPosition = MyGeometry.GetLocalSize() * 0.5f;
    
    FVector2D LocalTouchPos = MyGeometry.AbsoluteToLocal(InTouchEvent.GetScreenSpacePosition());
    FVector2D NewInput = CalculateInputFromPosition(MyGeometry, LocalTouchPos);
    UpdateInput(NewInput);
    
    return FReply::Handled();
}

FReply STitanPCVirtualJoystick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
    if (bIsPressed)
    {
        FVector2D LocalTouchPos = MyGeometry.AbsoluteToLocal(InTouchEvent.GetScreenSpacePosition());
        FVector2D NewInput = CalculateInputFromPosition(MyGeometry, LocalTouchPos);
        UpdateInput(NewInput);
        
        return FReply::Handled();
    }
    
    return FReply::Unhandled();
}

FReply STitanPCVirtualJoystick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
    if (bIsPressed)
    {
        bIsPressed = false;
        
        if (bSnapToCenter)
        {
            UpdateInput(FVector2D::ZeroVector);
        }
        
        return FReply::Handled();
    }
    
    return FReply::Unhandled();
}

void STitanPCVirtualJoystick::ResetInput()
{
    CurrentInput = FVector2D::ZeroVector;
    bIsPressed = false;
}

FVector2D STitanPCVirtualJoystick::CalculateInputFromPosition(const FGeometry& Geometry, const FVector2D& LocalPosition)
{
    const FVector2D LocalSize = Geometry.GetLocalSize();
    const FVector2D Center = LocalSize * 0.5f;
    const float MaxRadius = FMath::Min(LocalSize.X, LocalSize.Y) * 0.4f;
    
    FVector2D Delta = LocalPosition - Center;
    float Distance = Delta.Size();
    
    if (Distance < DeadZone * MaxRadius)
    {
        return FVector2D::ZeroVector;
    }
    
    // Normalize and clamp to max radius
    if (Distance > MaxRadius)
    {
        Delta = Delta.GetSafeNormal() * MaxRadius;
    }
    
    // Convert to normalized input (-1 to 1)
    FVector2D NormalizedInput = Delta / MaxRadius;
    return NormalizedInput * InputScale;
}

void STitanPCVirtualJoystick::UpdateInput(const FVector2D& NewInput)
{
    CurrentInput = NewInput;
    OnInputChanged.ExecuteIfBound(CurrentInput);
}