#pragma once

#include "InputDeviceEnum.generated.h"

UENUM(BlueprintType)
enum class DoorsInputDeviceEnum : uint8
{
    NONE = 0 UMETA(DisplayName = "None"),
    KEYBOARD = 1 UMETA(DisplayName = "Keyboard"),
    GAMEPAD = 2 UMETA(DisplayName = "Gamepad"),
};
