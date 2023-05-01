#include "PlayerStateSneaking.h"
#include "../InspectPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerStateSneaking::Enable()
{
    Super::Enable();

    CurrentMovementState = PlayerStateEnum::SNEAK_IDLE;
    PreviousMovementState = PlayerStateEnum::SNEAK_IDLE;

    IsCrouching = false;
    IsCrouchingOld = false;
    
    AInspectPlayer::Player->GetCharacterMovement()->MaxWalkSpeedCrouched = MaxSneakSpeed;

    AInspectPlayer::Player->Crouch();
    AInspectPlayer::Player->GetSpringArmCmp()->SetRelativeLocation({0.f, 0.f, AInspectPlayer::Player->CrouchedEyeHeight});
}

void UPlayerStateSneaking::Disable()
{
    Super::Disable();
}

void UPlayerStateSneaking::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // State by speed
    if (AInspectPlayer::Player->GetCharacterMovement()->Velocity.Size() > ConsideredWalking)
        CurrentMovementState = PlayerStateEnum::SNEAK_WALK;
    else
        CurrentMovementState = PlayerStateEnum::SNEAK_IDLE;

    // Change by state
    if (CurrentMovementState != PreviousMovementState)
        switch (CurrentMovementState)
        {
        case PlayerStateEnum::SNEAK_IDLE:
            AInspectPlayer::Player->SetState(PlayerStateEnum::SNEAK_IDLE);
            break;
        case PlayerStateEnum::SNEAK_WALK:
            AInspectPlayer::Player->SetState(PlayerStateEnum::SNEAK_WALK);
            break;
        }

    PreviousMovementState = CurrentMovementState;

    // Crouching
    IsCrouching = AInspectPlayer::Player->bIsCrouched;

    if (IsCrouching && !IsCrouchingOld)      // Started Crouching
        OnStartCrouching();                  //
    else if (!IsCrouching && IsCrouchingOld) // Stoped Crouching
        OnStopCrouching();                   //

    IsCrouchingOld = IsCrouching;
}

void UPlayerStateSneaking::ActionSneakPressed()
{
    Super::ActionSneakPressed();

    AInspectPlayer::Player->UnCrouch();
}

void UPlayerStateSneaking::ActionRunPressed()
{
    Super::ActionRunPressed();

    AInspectPlayer::Player->UnCrouch();
}

void UPlayerStateSneaking::OnStartCrouching()
{
}

void UPlayerStateSneaking::OnStopCrouching()
{
    AInspectPlayer::Player->GetSpringArmCmp()->SetRelativeLocation({0.f, 0.f, AInspectPlayer::Player->BaseEyeHeight});
    AInspectPlayer::Player->SetState(PlayerStateEnum::WANDER_IDLE, true);
}
