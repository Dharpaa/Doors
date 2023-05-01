#pragma once

#include "PlayerStateBase.h"
#include "PlayerStateInspecting.generated.h"

class UInteractInspectComponent;

UCLASS()
class UPlayerStateInspecting : public UPlayerStateBase
{
    GENERATED_BODY()

  protected:
    virtual void Enable() override;
    virtual void Disable() override;

  private:
	AActor* InspectingActor = nullptr;
	UInteractInspectComponent* InspectCompRef = nullptr;
};
