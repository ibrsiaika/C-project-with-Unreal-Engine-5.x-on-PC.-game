
#pragma once
#include "GameFramework/PlayerController.h"
#include "TitanPCPlayerController.generated.h"

UCLASS()
class TITANPC_API ATitanPCPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
};
