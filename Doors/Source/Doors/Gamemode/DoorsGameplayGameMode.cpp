#include "DoorsGameplayGameMode.h"
#include "Kismet/GameplayStatics.h"

ADoorsGameplayGameMode *ADoorsGameplayGameMode::Gameplay = nullptr;

ADoorsGameplayGameMode::ADoorsGameplayGameMode()
{
    Gameplay = this;
}

void ADoorsGameplayGameMode::BeginPlay()
{
    Super::BeginPlay();
}

// Privates

void ADoorsGameplayGameMode::OnAnyKey(FKey Key)
{
    auto OldInputDevice = CurrentInputDevice;

    if (Key.IsGamepadKey())
        OldInputDevice = DoorsInputDeviceEnum::GAMEPAD;
    else
        OldInputDevice = DoorsInputDeviceEnum::KEYBOARD;

    if (OldInputDevice != CurrentInputDevice)
    {
        CurrentInputDevice = OldInputDevice;
        InputDeviceDelegate.Broadcast(CurrentInputDevice);
    }
}

DoorsInputDeviceEnum ADoorsGameplayGameMode::GetInputDevice()
{
    return Gameplay ? Gameplay->CurrentInputDevice : DoorsInputDeviceEnum::NONE;
}
