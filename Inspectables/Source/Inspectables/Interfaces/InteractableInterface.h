#pragma once

#include "InteractableInterface.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class UInteractableInterface : public UInterface
{
    GENERATED_BODY()
};

class IInteractableInterface
{
    GENERATED_BODY()

  public:
    UFUNCTION(Category = "Interactable", BlueprintNativeEvent)
    bool IsInteractable();

    UFUNCTION(Category = "Interactable", BlueprintNativeEvent)
    void OnTap();

    UFUNCTION(Category = "Interactable", BlueprintNativeEvent)
    void OnCancel();

    UFUNCTION(Category = "Interactable", BlueprintNativeEvent)
    UTexture2D *GetTapIcon();

    UFUNCTION(Category = "Interactable", BlueprintNativeEvent)
    UTexture2D *GetCancelIcon();
};
