#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../Enumerators/InputDeviceEnum.h"
#include "DoorsGameplayGameMode.generated.h"

#define DIS 50000

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputDeviceEnumSignature, DoorsInputDeviceEnum, Device);

class UUserWidget;
class ADoorsPlayerController;
class AInspectCam;
class UItemAsset;

UCLASS()
class ADoorsGameplayGameMode : public AGameModeBase
{
    GENERATED_BODY()

  public:
    static ADoorsGameplayGameMode *Gameplay;

    ADoorsGameplayGameMode();

    virtual void BeginPlay() override;

    void OnAnyKey(FKey Key);

    UFUNCTION(BlueprintPure, BlueprintCallable)
    static DoorsInputDeviceEnum GetInputDevice();

    UPROPERTY(BlueprintAssignable)
    FInputDeviceEnumSignature InputDeviceDelegate;

  private:
    DoorsInputDeviceEnum CurrentInputDevice = DoorsInputDeviceEnum::NONE;
	
};
