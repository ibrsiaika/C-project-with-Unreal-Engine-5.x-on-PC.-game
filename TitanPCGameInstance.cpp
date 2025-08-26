
#include "TitanPCGameInstance.h"
#include "Engine/Engine.h"
#include "HAL/ConsoleManager.h"

void UTitanPCGameInstance::Init()
{
    Super::Init();
    SetTargetFramerate(TargetFPS);
}

void UTitanPCGameInstance::SetTargetFramerate(int32 FPS)
{
    TargetFPS = FMath::Clamp(FPS, 30, 240);
    IConsoleVariable* MaxFPSCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
    if (MaxFPSCVar)
    {
        MaxFPSCVar->Set(TargetFPS, ECVF_SetByGameSetting);
    }
}
