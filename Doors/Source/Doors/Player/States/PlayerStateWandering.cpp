#include "PlayerStateWandering.h"
#include "../Player.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UPlayerStateWandering::Enable()
{
    Super::Enable();

    CurrentMovementState = PlayerStateEnum::WANDER_IDLE;
    PreviousMovementState = PlayerStateEnum::WANDER_IDLE;
    
    APlayer::MskPlayer->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

    if (APlayer::MskPlayer->bActionRunState)
        ActionRunPressed();
}

void UPlayerStateWandering::Disable()
{
    Super::Disable();

    APlayer::MskPlayer->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void UPlayerStateWandering::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Run backwards check
    auto MoveAxis = APlayer::MskPlayer->GetMovementAxis();

    if (APlayer::MskPlayer->bActionRunState)
        if (MoveAxis.Y < 0.f)
            APlayer::MskPlayer->SetMovementAxis(MoveAxis / BackwardsRunPenalty);

    // State by speed
    if (APlayer::MskPlayer->GetCharacterMovement()->Velocity.Size() > ConsideredRunning)
        CurrentMovementState = MskPlayerStateEnum::WANDER_RUN;
    else if (APlayer::MskPlayer->GetCharacterMovement()->Velocity.Size() > ConsideredWalking)
        CurrentMovementState = MskPlayerStateEnum::WANDER_WALK;
    else
        CurrentMovementState = MskPlayerStateEnum::WANDER_IDLE;

    // Change by state
    if (CurrentMovementState != PreviousMovementState)
        switch (CurrentMovementState)
        {
        case MskPlayerStateEnum::WANDER_IDLE:
            APlayer::MskPlayer->StartHeadBob(HeadBobIdle);
            APlayer::MskPlayer->SetCanLean(CanLeanWhileIdle);
            APlayer::MskPlayer->SetState(MskPlayerStateEnum::WANDER_IDLE);
            break;
        case MskPlayerStateEnum::WANDER_WALK:
            APlayer::MskPlayer->StartHeadBob(HeadBobWalk, true);
            APlayer::MskPlayer->SetCanLean(CanLeanWhileWalking);
            APlayer::MskPlayer->SetState(MskPlayerStateEnum::WANDER_WALK);
            break;
        case MskPlayerStateEnum::WANDER_RUN:
            APlayer::MskPlayer->StartHeadBob(HeadBobRun, true);
            APlayer::MskPlayer->SetCanLean(CanLeanWhileRunning);
            APlayer::MskPlayer->SetState(MskPlayerStateEnum::WANDER_RUN);
            break;
        }

    // Update previous state
    PreviousMovementState = CurrentMovementState;
}

void UPlayerStateWandering::ActionRunPressed()
{
    Super::ActionRunPressed();

    APlayer::MskPlayer->GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
}

void UPlayerStateWandering::ActionRunReleased()
{
    Super::ActionRunPressed();

    APlayer::MskPlayer->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void UPlayerStateWandering::ActionSneakPressed()
{
    Super::ActionSneakPressed();

    APlayer::MskPlayer->SetState(MskPlayerStateEnum::SNEAK_IDLE, true);
}
