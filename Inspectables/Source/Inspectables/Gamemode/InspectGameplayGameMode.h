#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../Enumerators/InputDeviceEnum.h"
#include "InspectGameplayGameMode.generated.h"

#define DIS 50000

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputDeviceEnumSignature, InputDeviceEnum, Device);

class UUserWidget;
class ADoorsPlayerController;
class AInspectCam;
class UItemAsset;

UCLASS()
class AInspectGameplayGameMode : public AGameModeBase
{
    GENERATED_BODY()

  public:
    static AInspectGameplayGameMode *Gameplay;

    AInspectGameplayGameMode();

    virtual void BeginPlay() override;

    void OnAnyKey(FKey Key);

    UFUNCTION(BlueprintPure, BlueprintCallable)
    static InputDeviceEnum GetInputDevice();

    UPROPERTY(BlueprintAssignable)
    FInputDeviceEnumSignature InputDeviceDelegate;

  private:
    InputDeviceEnum CurrentInputDevice = InputDeviceEnum::NONE;
	
};
