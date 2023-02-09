#pragma once

#include "CoreMinimal.h"
#include "PlayerStateBase.generated.h"

class APlayer;

UCLASS(EditInlineNew, DefaultToInstanced)
class UPlayerStateBase : public UObject
{
	GENERATED_BODY()

	friend APlayer;

protected:
	virtual void Enable();
	virtual void Disable();

	virtual void Tick(float DeltaTime);

	virtual void ActionRunPressed();
	virtual void ActionRunReleased();

	virtual void ActionSneakPressed();
	virtual void ActionSneakReleased();
};
