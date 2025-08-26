
#include "TitanPCGameModeBase.h"
#include "TitanPCCharacter.h"
#include "TitanPCPlayerController.h"

ATitanPCGameModeBase::ATitanPCGameModeBase()
{
    DefaultPawnClass = ATitanPCCharacter::StaticClass();
    PlayerControllerClass = ATitanPCPlayerController::StaticClass();
}
