#include "PlayerStateWandering.h"
#include "../DoorsPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UPlayerStateWandering::Enable()
{
    Super::Enable();

    CurrentMovementState = PlayerStateEnum::WANDER_IDLE;
    PreviousMovementState = PlayerStateEnum::WANDER_IDLE;
    
    ADoorsPlayer::DoorsPlayer->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

    if (ADoorsPlayer::DoorsPlayer->bActionRunState)
        ActionRunPressed();
}

void UPlayerStateWandering::Disable()
{
    Super::Disable();

    ADoorsPlayer::DoorsPlayer->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void UPlayerStateWandering::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Run backwards check
    auto MoveAxis = ADoorsPlayer::DoorsPlayer->GetMovementAxis();

    if (ADoorsPlayer::DoorsPlayer->bActionRunState)
        if (MoveAxis.Y < 0.f)
            ADoorsPlayer::DoorsPlayer->SetMovementAxis(MoveAxis / BackwardsRunPenalty);

    // State by speed
    if (ADoorsPlayer::DoorsPlayer->GetCharacterMovement()->Velocity.Size() > ConsideredRunning)
        CurrentMovementState = PlayerStateEnum::WANDER_RUN;
    else if (ADoorsPlayer::DoorsPlayer->GetCharacterMovement()->Velocity.Size() > ConsideredWalking)
        CurrentMovementState = PlayerStateEnum::WANDER_WALK;
    else
        CurrentMovementState = PlayerStateEnum::WANDER_IDLE;

    // Change by state
    if (CurrentMovementState != PreviousMovementState)
        switch (CurrentMovementState)
        {
        case PlayerStateEnum::WANDER_IDLE:
            ADoorsPlayer::DoorsPlayer->SetState(PlayerStateEnum::WANDER_IDLE);
            break;
        case PlayerStateEnum::WANDER_WALK:
            ADoorsPlayer::DoorsPlayer->SetState(PlayerStateEnum::WANDER_WALK);
            break;
        case PlayerStateEnum::WANDER_RUN:
            ADoorsPlayer::DoorsPlayer->SetState(PlayerStateEnum::WANDER_RUN);
            break;
        }

    // Update previous state
    PreviousMovementState = CurrentMovementState;
}

void UPlayerStateWandering::ActionRunPressed()
{
    Super::ActionRunPressed();

    ADoorsPlayer::DoorsPlayer->GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
}

void UPlayerStateWandering::ActionRunReleased()
{
    Super::ActionRunPressed();

    ADoorsPlayer::DoorsPlayer->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void UPlayerStateWandering::ActionSneakPressed()
{
    Super::ActionSneakPressed();

    ADoorsPlayer::DoorsPlayer->SetState(PlayerStateEnum::SNEAK_IDLE, true);
}
