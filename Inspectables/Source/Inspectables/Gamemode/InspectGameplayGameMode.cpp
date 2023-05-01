#include "InspectGameplayGameMode.h"
#include "Kismet/GameplayStatics.h"

AInspectGameplayGameMode *AInspectGameplayGameMode::Gameplay = nullptr;

AInspectGameplayGameMode::AInspectGameplayGameMode()
{
    Gameplay = this;
}

void AInspectGameplayGameMode::BeginPlay()
{
    Super::BeginPlay();
}

// Privates

void AInspectGameplayGameMode::OnAnyKey(FKey Key)
{
    auto OldInputDevice = CurrentInputDevice;

    if (Key.IsGamepadKey())
        OldInputDevice = InputDeviceEnum::GAMEPAD;
    else
        OldInputDevice = InputDeviceEnum::KEYBOARD;

    if (OldInputDevice != CurrentInputDevice)
    {
        CurrentInputDevice = OldInputDevice;
        InputDeviceDelegate.Broadcast(CurrentInputDevice);
    }
}

InputDeviceEnum AInspectGameplayGameMode::GetInputDevice()
{
    return Gameplay ? Gameplay->CurrentInputDevice : InputDeviceEnum::NONE;
}
