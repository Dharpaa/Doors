#pragma once

#include "CoreMinimal.h"
#include "PlayerStateBase.generated.h"

class ADoorsPlayer;

UCLASS(EditInlineNew, DefaultToInstanced)
class UPlayerStateBase : public UObject
{
	GENERATED_BODY()

	friend ADoorsPlayer;

protected:
	virtual void Enable();
	virtual void Disable();

	virtual void Tick(float DeltaTime);

	virtual void ActionRunPressed();
	virtual void ActionRunReleased();

	virtual void ActionSneakPressed();
	virtual void ActionSneakReleased();
};
