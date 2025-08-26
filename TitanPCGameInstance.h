
#pragma once
#include "Engine/GameInstance.h"
#include "TitanPCGameInstance.generated.h"

UCLASS()
class TITANPC_API UTitanPCGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION(BlueprintCallable, Category="Project")
    void SetTargetFramerate(int32 FPS);

    UFUNCTION(BlueprintPure, Category="Project")
    int32 GetTargetFramerate() const { return TargetFPS; }

private:
    int32 TargetFPS = 120;
};
