
#include "TitanPCPlayerController.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"

void ATitanPCPlayerController::BeginPlay()
{
    Super::BeginPlay();
    bShowMouseCursor = false;
    FInputModeGameOnly Mode;
    SetInputMode(Mode);
}
