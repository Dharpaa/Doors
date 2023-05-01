#pragma once

#include "InteractableByAIInterface.generated.h"

UINTERFACE(BlueprintType)
class UInteractableByAIInterface : public UInterface
{
    GENERATED_BODY()
};

class IInteractableByAIInterface
{
    GENERATED_BODY()
  public:
    UFUNCTION(Category = Interactable, BlueprintCallable, BlueprintNativeEvent)
    void OnInteractAIStart();

    UFUNCTION(Category = Interactable, BlueprintCallable, BlueprintNativeEvent)
    void OnInteractAIStop();

    UFUNCTION(Category = Interactable, BlueprintCallable, BlueprintNativeEvent)
    void OnResetInteractables();

    UFUNCTION(Category = Interactable, BlueprintCallable, BlueprintNativeEvent)
    /** This Function Returns an AnimMontage to play when the enemy reaches the object */
    UAnimMontage *InteractAnimMontage();
};