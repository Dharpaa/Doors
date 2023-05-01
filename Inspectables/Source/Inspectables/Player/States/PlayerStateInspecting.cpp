#include "PlayerStateInspecting.h"
#include "../InspectPlayer.h"
#include "Camera/CameraComponent.h"
#include "../../Components/InteractInspectComponent.h"

void UPlayerStateInspecting::Enable()
{
    UPlayerStateBase::Enable();

    AInspectPlayer::Player->BlockMovement(999999.0);
    AInspectPlayer::Player->BlockInteract(999999.0);
    AInspectPlayer::Player->BlockLooking(999999.0);

    InspectingActor = Cast<AActor>(AInspectPlayer::Player->DetectedInteractable);
    if (InspectingActor)
    {
        InspectCompRef = static_cast<UInteractInspectComponent*>(InspectingActor->GetComponentByClass(UInteractInspectComponent::StaticClass()));
    }
}

void UPlayerStateInspecting::Disable()
{
    UPlayerStateBase::Disable();

    AInspectPlayer::Player->RestoreMovement();
    AInspectPlayer::Player->RestoreInteract();
    AInspectPlayer::Player->RestoreLooking();

    InspectCompRef->CancelInspect();
}
