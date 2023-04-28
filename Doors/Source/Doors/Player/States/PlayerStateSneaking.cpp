#include "PlayerStateSneaking.h"
#include "../DoorsPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerStateSneaking::Enable()
{
    Super::Enable();

    CurrentMovementState = PlayerStateEnum::SNEAK_IDLE;
    PreviousMovementState = PlayerStateEnum::SNEAK_IDLE;

    IsCrouching = false;
    IsCrouchingOld = false;
    
    ADoorsPlayer::DoorsPlayer->GetCharacterMovement()->MaxWalkSpeedCrouched = MaxSneakSpeed;

    ADoorsPlayer::DoorsPlayer->Crouch();
    ADoorsPlayer::DoorsPlayer->GetSpringArmCmp()->SetRelativeLocation({0.f, 0.f, ADoorsPlayer::DoorsPlayer->CrouchedEyeHeight});
}

void UPlayerStateSneaking::Disable()
{
    Super::Disable();
}

void UPlayerStateSneaking::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // State by speed
    if (ADoorsPlayer::DoorsPlayer->GetCharacterMovement()->Velocity.Size() > ConsideredWalking)
        CurrentMovementState = PlayerStateEnum::SNEAK_WALK;
    else
        CurrentMovementState = PlayerStateEnum::SNEAK_IDLE;

    // Change by state
    if (CurrentMovementState != PreviousMovementState)
        switch (CurrentMovementState)
        {
        case PlayerStateEnum::SNEAK_IDLE:
            ADoorsPlayer::DoorsPlayer->SetState(PlayerStateEnum::SNEAK_IDLE);
            break;
        case PlayerStateEnum::SNEAK_WALK:
            ADoorsPlayer::DoorsPlayer->SetState(PlayerStateEnum::SNEAK_WALK);
            break;
        }

    PreviousMovementState = CurrentMovementState;

    // Crouching
    IsCrouching = ADoorsPlayer::DoorsPlayer->bIsCrouched;

    if (IsCrouching && !IsCrouchingOld)      // Started Crouching
        OnStartCrouching();                  //
    else if (!IsCrouching && IsCrouchingOld) // Stoped Crouching
        OnStopCrouching();                   //

    IsCrouchingOld = IsCrouching;
}

void UPlayerStateSneaking::ActionSneakPressed()
{
    Super::ActionSneakPressed();

    ADoorsPlayer::DoorsPlayer->UnCrouch();
}

void UPlayerStateSneaking::ActionRunPressed()
{
    Super::ActionRunPressed();

    ADoorsPlayer::DoorsPlayer->UnCrouch();
}

void UPlayerStateSneaking::OnStartCrouching()
{
}

void UPlayerStateSneaking::OnStopCrouching()
{
    ADoorsPlayer::DoorsPlayer->GetSpringArmCmp()->SetRelativeLocation({0.f, 0.f, ADoorsPlayer::DoorsPlayer->BaseEyeHeight});
    ADoorsPlayer::DoorsPlayer->SetState(PlayerStateEnum::WANDER_IDLE, true);
}
