#pragma once

#include "PlayerStateEnum.generated.h"

UENUM(BlueprintType)
enum class PlayerStateEnum : uint8
{
	NONE = 0 UMETA(DisplayName = "None"),
	DEFAULT = 1 UMETA(DisplayName = "Default"),
	WANDER_IDLE = 2 UMETA(DisplayName = "Wander Idle"),
	WANDER_WALK = 3 UMETA(DisplayName = "Wander Walk"),
	WANDER_RUN = 4 UMETA(DisplayName = "Wander Run"),
	SNEAK_IDLE = 5 UMETA(DisplayName = "Sneak Idle"),
	SNEAK_WALK = 6 UMETA(DisplayName = "Sneak Walk")
};
