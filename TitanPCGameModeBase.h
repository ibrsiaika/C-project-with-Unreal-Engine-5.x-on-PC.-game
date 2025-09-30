
#pragma once
#include "GameFramework/GameModeBase.h"
#include "TitanPCGameModeBase.generated.h"

class ATitanPCDropShipSequence;
class ATitanPCCharacter;
class APlayerController;

UCLASS()
class TITANPC_API ATitanPCGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATitanPCGameModeBase();

protected:
    virtual void BeginPlay() override;

    // Optional: set to your custom cinematic class in defaults or via BP
    UPROPERTY(EditDefaultsOnly, Category="Cinematic")
    TSubclassOf<ATitanPCDropShipSequence> DropShipSequenceClass;

    UPROPERTY()
    ATitanPCDropShipSequence* ActiveDropSequence = nullptr;

    void StartBattleRoyaleIntro();
};
