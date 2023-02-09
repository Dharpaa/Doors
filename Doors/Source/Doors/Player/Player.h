#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Enumerators/PlayerStateEnum.h"
#include "Player.generated.h"

class UPlayerStateSneaking;
class UPlayerStateWandering;
class UPlayerStateBase;
class USpringArmComponent;
class UCameraComponent;


UCLASS()
class APlayer : public ACharacter
{
	GENERATED_BODY()

	public:
		APlayer();

	private:
		virtual void BeginPlay() override;
		virtual void Tick(float DeltaTime) override;
		virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;
	
	public:
		static APlayer *MskPlayer;
 
		UFUNCTION(Category = "Components", BlueprintCallable)
		/** Get camera component.
		 * @return The Camera component. */
		UCameraComponent *GetCameraCmp();

		UFUNCTION(Category = "Components", BlueprintCallable)
		/** Get the spring arm component that holds the camera component.
		 * @return The SpringArm component. */
		USpringArmComponent *GetSpringArmCmp();

		UFUNCTION(Category = "Components", BlueprintCallable)
		/** Get player controller.
		 * @return The Player Controller. */
		APlayerController *GetPlayerController();

		UFUNCTION(Category = "Components", BlueprintCallable)
		/** Get the camera manager that controls the camera component.
		 * @return The Player Camera Manager. */
		APlayerCameraManager *GetCameraManager();

	//??
		UPROPERTY(Category = "Player|Sensibility", EditAnywhere, BlueprintReadWrite)
		float fPitchSensibility = 80.f;
		UPROPERTY(Category = "Player|Sensibility", EditAnywhere, BlueprintReadWrite)
		float fYawSensibility = 90.f;

	private:
		UPROPERTY(Category = "Player", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent *SpringArmCmp;

		UPROPERTY(Category = "Player", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent *CameraCmp;

		UPROPERTY(Category = "Player", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraShakeSourceComponent *CameraShakeCmp;
	
		UPROPERTY(Category = "Player", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent *HandSocketCmp;

		APlayerController *PlayerController = nullptr;

		APlayerCameraManager *PlayerCameraManager = nullptr;

	// States

	public:
	UFUNCTION(Category = "States", BlueprintCallable)
	/** Returns the current state of the player.
	 * @return The current state of the player. */
	PlayerStateEnum GetState();

	UFUNCTION(Category = "States", BlueprintCallable)
	/** Set the state of the player.
	 * @param NewState New state to switch to.
	 * @param IsSuperState Whether the new state should restart the state. */
	void SetState(PlayerStateEnum NewState, bool IsSuperState = false);

	UFUNCTION(Category = "States", BlueprintImplementableEvent)
	/** Event that triggers each time the player state changes.
	 * @return The new state. */
	void OnStateChanged(PlayerStateEnum NewState);

	bool wasSneaking;

	private:
		UPROPERTY(Category = "States", EditAnywhere, Instanced)
		UPlayerStateWandering *StateWandering;

		UPROPERTY(Category = "States", EditAnywhere, Instanced)
		UPlayerStateSneaking *StateSneaking;
	

		PlayerStateEnum CurrentStateEnum = PlayerStateEnum::WANDER_IDLE;

		UPlayerStateBase *CurrentState;

		UPROPERTY(Category = "States", EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool ShowDebugStateChanges = false;
};
