
#include "TitanPCGameModeBase.h"
#include "TitanPCCharacter.h"
#include "TitanPCPlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Cinematics/TitanPCDropShipSequence.h"

ATitanPCGameModeBase::ATitanPCGameModeBase()
{
    DefaultPawnClass = ATitanPCCharacter::StaticClass();
    PlayerControllerClass = ATitanPCPlayerController::StaticClass();
}

void ATitanPCGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    StartBattleRoyaleIntro();
}

void ATitanPCGameModeBase::StartBattleRoyaleIntro()
{
    if (!DropShipSequenceClass)
    {
        // You can assign DropShipSequenceClass in the GameMode BP or DefaultGame.ini.
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    ATitanPCCharacter* Character = Cast<ATitanPCCharacter>(UGameplayStatics::GetPlayerPawn(World, 0));
    if (!PC || !Character) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ActiveDropSequence = World->SpawnActor<ATitanPCDropShipSequence>(DropShipSequenceClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
    if (!ActiveDropSequence) return;

    ActiveDropSequence->StartSequence(PC, Character);
}
