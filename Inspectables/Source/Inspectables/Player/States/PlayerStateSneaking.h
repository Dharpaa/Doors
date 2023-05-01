#pragma once

#include "PlayerStateBase.h"
#include "Doors/Enumerators/PlayerStateEnum.h"
#include "PlayerStateSneaking.generated.h"

class UCamShake;

UCLASS()
class UPlayerStateSneaking : public UPlayerStateBase
{
	GENERATED_BODY()

protected:
	virtual void Enable() override;
	virtual void Disable() override;

	virtual void Tick(float DeltaTime) override;

	virtual void ActionSneakPressed() override;
	virtual void ActionRunPressed() override;

private:
	// State handling

	PlayerStateEnum CurrentMovementState = PlayerStateEnum::WANDER_IDLE;
	PlayerStateEnum PreviousMovementState = PlayerStateEnum::WANDER_IDLE;

	// Movement

	UPROPERTY(Category = State, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxSneakSpeed = 100.f;

	UPROPERTY(Category = State, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	/** Minimun speed to be considered as movement. Used for head bobbing selection. */
	float ConsideredWalking = 50.f;

	// Crouch

	void OnStartCrouching();
	void OnStopCrouching();

	bool IsCrouching = false;
	bool IsCrouchingOld = false;
};
