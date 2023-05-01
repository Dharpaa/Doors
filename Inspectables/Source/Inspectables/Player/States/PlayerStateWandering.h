#pragma once

#include "PlayerStateBase.h"
#include "Doors/Enumerators/PlayerStateEnum.h"
#include "PlayerStateWandering.generated.h"

class UCamShake;
class ULeanParams;

UCLASS()
class UPlayerStateWandering : public UPlayerStateBase
{
    GENERATED_BODY()

  protected:
    virtual void Enable() override;
    virtual void Disable() override;

    virtual void Tick(float DeltaTime) override;

    virtual void ActionRunPressed() override;
    virtual void ActionRunReleased() override;

    virtual void ActionSneakPressed() override;

  private:
    // State handling

    PlayerStateEnum CurrentMovementState = PlayerStateEnum::WANDER_IDLE;
    PlayerStateEnum PreviousMovementState = PlayerStateEnum::WANDER_IDLE;

    // Movement

    UPROPERTY(Category = State, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float MaxWalkSpeed = 200.f;

    UPROPERTY(Category = State, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float MaxRunSpeed = 350.f;

	UPROPERTY(Category = State, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	/** Minimun speed to be considered as movement. Used for head bobbing selection. */
	float ConsideredWalking = 100.f;

	UPROPERTY(Category = State, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	/** Minimun speed to be considered as movement. Used for head bobbing selection. */
	float ConsideredRunning = 300.f;
	
    UPROPERTY(Category = State, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    /** Movement penalty multiplier applied to forwards axis of movement when running backwards. */
    float BackwardsRunPenalty = 3.f;
};
