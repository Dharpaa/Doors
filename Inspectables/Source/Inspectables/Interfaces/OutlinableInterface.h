#pragma once

#include "OutlinableInterface.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class UOutlinableInterface : public UInterface
{
    GENERATED_BODY()
};

class IOutlinableInterface
{
    GENERATED_BODY()

  public:
    UFUNCTION(Category = "Outlinable", BlueprintNativeEvent)
    bool IsOutlinable();

    UFUNCTION(Category = "Outlinable", BlueprintNativeEvent)
    bool KeepOutline();

    UFUNCTION(Category = "Outlinable", BlueprintNativeEvent)
    TArray<UStaticMeshComponent *> GetMeshes();
};
