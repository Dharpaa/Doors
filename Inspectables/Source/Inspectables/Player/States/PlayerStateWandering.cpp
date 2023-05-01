#include "PlayerStateWandering.h"
#include "../InspectPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UPlayerStateWandering::Enable()
{
    Super::Enable();

    CurrentMovementState = PlayerStateEnum::WANDER_IDLE;
    PreviousMovementState = PlayerStateEnum::WANDER_IDLE;
    
    AInspectPlayer::Player->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

    if (AInspectPlayer::Player->bActionRunState)
        ActionRunPressed();
}

void UPlayerStateWandering::Disable()
{
    Super::Disable();

    AInspectPlayer::Player->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void UPlayerStateWandering::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Run backwards check
    auto MoveAxis = AInspectPlayer::Player->GetMovementAxis();

    if (AInspectPlayer::Player->bActionRunState)
        if (MoveAxis.Y < 0.f)
            AInspectPlayer::Player->SetMovementAxis(MoveAxis / BackwardsRunPenalty);

    // State by speed
    if (AInspectPlayer::Player->GetCharacterMovement()->Velocity.Size() > ConsideredRunning)
        CurrentMovementState = PlayerStateEnum::WANDER_RUN;
    else if (AInspectPlayer::Player->GetCharacterMovement()->Velocity.Size() > ConsideredWalking)
        CurrentMovementState = PlayerStateEnum::WANDER_WALK;
    else
        CurrentMovementState = PlayerStateEnum::WANDER_IDLE;

    // Change by state
    if (CurrentMovementState != PreviousMovementState)
        switch (CurrentMovementState)
        {
        case PlayerStateEnum::WANDER_IDLE:
            AInspectPlayer::Player->SetState(PlayerStateEnum::WANDER_IDLE);
            break;
        case PlayerStateEnum::WANDER_WALK:
            AInspectPlayer::Player->SetState(PlayerStateEnum::WANDER_WALK);
            break;
        case PlayerStateEnum::WANDER_RUN:
            AInspectPlayer::Player->SetState(PlayerStateEnum::WANDER_RUN);
            break;
        }

    // Update previous state
    PreviousMovementState = CurrentMovementState;
}

void UPlayerStateWandering::ActionRunPressed()
{
    Super::ActionRunPressed();

    AInspectPlayer::Player->GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
}

void UPlayerStateWandering::ActionRunReleased()
{
    Super::ActionRunPressed();

    AInspectPlayer::Player->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void UPlayerStateWandering::ActionSneakPressed()
{
    Super::ActionSneakPressed();

    AInspectPlayer::Player->SetState(PlayerStateEnum::SNEAK_IDLE, true);
}
