#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Enumerators/PlayerStateEnum.h"
#include "DoorsPlayer.generated.h"

class ADoorsGameplayGameMode;
class ADoorsPlayerController;
class UPlayerStateSneaking;
class UPlayerStateWandering;
class UPlayerStateBase;
class USpringArmComponent;
class UCameraComponent;

USTRUCT(BlueprintType)
struct FDoorsPlayerResponse
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Player"))
	ADoorsPlayer *Player = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "IsValid"))
	bool bIsValid = false;

	// Constructor
	FDoorsPlayerResponse(ADoorsPlayer *_Player = nullptr, bool _bIsValid = false)
	{
		Player = _Player;
		bIsValid = _bIsValid;
	}
};

UCLASS()
class ADoorsPlayer : public ACharacter
{
	GENERATED_BODY()

	public:
		ADoorsPlayer();

	private:
		virtual void BeginPlay() override;
		virtual void Tick(float DeltaTime) override;
		virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;

	//Components
	
	public:
		static ADoorsPlayer *DoorsPlayer;

		UFUNCTION(Category = "Utils", BlueprintPure)
	   /** Get the player and its state.
		* @return Struct that contains the player and if it's valid or not. */
	   static UPARAM(DisplayName = "‎") FDoorsPlayerResponse GetDoorsPlayer();
	
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
		ADoorsPlayerController *GetPlayerController();

		UFUNCTION(Category = "Components", BlueprintCallable)
		/** Get the camera manager that controls the camera component.
		 * @return The Player Camera Manager. */
		APlayerCameraManager *GetCameraManager();

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

		ADoorsGameplayGameMode *GameMode = nullptr;
		ADoorsPlayerController *PlayerController = nullptr;

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

	// Interactable Detection

	  public:
	    UFUNCTION(Category = "Interactables|Rays", BlueprintCallable)
	    /** Sets the trace channel to use when searching for interactables.
	     * @param Channel Channel to collide with. */
	    void SetTraceChannel(ETraceTypeQuery Channel);

	    UFUNCTION(Category = "Interactables|Rays", BlueprintCallable)
	    /** Reset the trace channel by the original one (ETraceTypeQuery::TraceTypeQuery1). */
	    void ResetTraceChannel();

		UFUNCTION(Category = "Interactables|Rays", BlueprintImplementableEvent)
		void OnInteractableFound();

		UFUNCTION(Category = "Interactables|Rays", BlueprintImplementableEvent)
		void OnInteractableLost();

	    UFUNCTION(Category = "Interactables|Tap", BlueprintImplementableEvent)
	    /** Event that triggers when player left click an interactable. */
	    void OnTap();

	    UFUNCTION(Category = "Interactables|Tap", BlueprintImplementableEvent)
	    /** Event that triggers when player right click an interactable. */
	    void OnCancel();
	
		

	  private:
	    UPROPERTY(Category = "Interactables|Rays", EditDefaultsOnly, BlueprintReadWrite,
	              meta = (AllowPrivateAccess = "true"))
	    float BigRayRadius = 100.f;

	    UPROPERTY(Category = "Interactables|Rays", EditDefaultsOnly, BlueprintReadWrite,
	              meta = (AllowPrivateAccess = "true"))
	    float BigRayLength = 100.f;

	    UPROPERTY(Category = "Interactables|Rays", EditDefaultsOnly, BlueprintReadWrite,
	              meta = (AllowPrivateAccess = "true"))
	    float SmallRayRadius = 100.f;

	    UPROPERTY(Category = "Interactables|Rays", EditDefaultsOnly, BlueprintReadWrite,
	              meta = (AllowPrivateAccess = "true"))
	    float SmallRayLength = 100.f;

	    UPROPERTY(Category = "Interactables|Rays", EditDefaultsOnly, BlueprintReadWrite,
	              meta = (AllowPrivateAccess = "true"))
	    bool ShowDebugInteractables = false;

	    UPROPERTY(Category = "Interactables|Tap", EditDefaultsOnly, BlueprintReadWrite,
	              meta = (AllowPrivateAccess = "true"))
	    bool ShowDebugTap = false;

	    void SearchInteractables();

	    void InteractableFound(UObject *New);
	    void InteractableChanged(UObject *New, UObject *Old);
	    void InteractableLost(UObject *Old);

	    void SetOutline(UObject *Obj, bool Value);
	

	    UObject *DetectedInteractable = nullptr;    // Interactable currently being detected by the ray
	    UObject *OldDetectedInteractable = nullptr; // Last frame DetectedInteractable

	    ETraceTypeQuery CurrentTraceChannel = ETraceTypeQuery::TraceTypeQuery1;
	
	//Input
	public:
		FVector2D GetMovementAxis();
		void SetMovementAxis(FVector2D Value);

		FVector2D GetLookAxis();
		void SetLookAxis(FVector2D Value);

		bool bActionRunState = false;
		bool bActionSneakState = false;
		bool bActionInteractState = false;

	private:
		void AxisPitch(float Amount);
		void AxisYaw(float Amount);

		void AxisForwards(float Amount);
		void AxisRight(float Amount);

		void ActionRunPressed();
		void ActionRunReleased();

		void ActionSneakPressed();
		void ActionSneakReleased();

		void ActionInteractPressed();
		void ActionInteractReleased();

		void Move();
		void Look(float DeltaTime);

		FVector2D MoveAxis = FVector2D::ZeroVector;
		FVector2D LookAxis = FVector2D::ZeroVector;

		UFUNCTION() void OnAnyKey(FKey Key);
};
