#pragma once

#include "CoreMinimal.h"
#include "AIDoorsController.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "AIEnemyController.generated.h"

UENUM()
enum class EnemyStates : uint8
{
	Normal		= 0 UMETA(DisplayName = "Normal"),
	Aware		= 1 UMETA(DisplayName = "Aware"),
	Looking		= 2 UMETA(DisplayName = "Looking"),
	Chasing		= 3 UMETA(DisplayName = "Chasing"),
	Returning	= 4 UMETA(DisplayName = "Returning")
};

class UAISenseConfig_Sight;
class ADoorsPlayer;

UCLASS()
class AAIEnemyController : public AAIDoorsController
{
	GENERATED_BODY()

private:
	UAISenseConfig_Sight* SightConfig;
	UAISenseConfig_Hearing* HearingConfig;

	TArray<class AMskInteractableBase*> Interactables;

	ADoorsPlayer* PlayerRef;

public:
	AAIEnemyController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	ADoorsPlayer* GetPlayerRef() { return PlayerRef; }

	UPROPERTY(VisibleDefaultsOnly, Category = AIPerception)
	UAIPerceptionComponent* AIPerceptionComponent;

	void SetUpPerception();

	UFUNCTION(BlueprintCallable)
	void SetUpPerceptionValues();

	UFUNCTION(BlueprintCallable)
	void SetPerceptionToZero();

	UPROPERTY(Category = "AIPerception", EditAnywhere, BlueprintReadWrite)
		float VisionRadiusDetect = 600.f;
	UPROPERTY(Category = "AIPerception", EditAnywhere, BlueprintReadWrite)
		float VisionRadiusLose = 800.f;
	UPROPERTY(Category = "AIPerception", EditAnywhere, BlueprintReadWrite)
		float VisionPeriphericalRadius = 90.f;
	UPROPERTY(Category = "AIPerception", EditAnywhere, BlueprintReadWrite)
		float HearRadius = 1200.f;
	
	UPROPERTY(Category = "AIPerception", EditAnywhere, BlueprintReadWrite)
		float fMaxDistanceAttenuationWall = 750.f;
	UPROPERTY(Category = "AIPerception", EditAnywhere, BlueprintReadWrite)
		int iMaxWallsToStopHearing = 2;
	UPROPERTY(Category = "AIPerception", EditAnywhere, BlueprintReadWrite)
		float TimeRemaindingPlayer_Max = 5.f;

	// Binding Perception function
	UFUNCTION()
		void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EnemyStates CurrentState = EnemyStates::Normal;

	// Alarm Thresholds
	UPROPERTY(Category = "AlarmThresholds", EditAnywhere, BlueprintReadWrite)
		float AlarmLevel_Aware;
	UPROPERTY(Category = "AlarmThresholds", EditAnywhere, BlueprintReadWrite)
		float AlarmLevel_Looking;
	UPROPERTY(Category = "AlarmThresholds", EditAnywhere, BlueprintReadWrite)
		float AlarmLevel_Chasing;
	UPROPERTY(Category = "AlarmThresholds", EditAnywhere, BlueprintReadWrite)
		float AlarmMaximumValue;

	// Alarm Modifiers
	UPROPERTY(Category = "Alarm Factor", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_Factor = 1.f;
	UPROPERTY(Category = "Alarm Sight", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_DistanceSight;
	UPROPERTY(Category = "Alarm Hearing", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_DistanceHearing;
	UPROPERTY(Category = "Alarm Sight", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_AngleSight;
	UPROPERTY(Category = "Alarm Sight", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_PlayerRunningSight;
	UPROPERTY(Category = "Alarm Sight", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_PlayerWalkingSight;
	UPROPERTY(Category = "Alarm Sight", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_PlayerSneakingSight;
	UPROPERTY(Category = "Alarm Sight", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_PointsSight = 1.f;
	UPROPERTY(Category = "Alarm Hearing", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_PlayerRunningHearing;
	UPROPERTY(Category = "Alarm Hearing", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_PlayerWalkingHearing;

	// Size of the sweep sphere for checking visible points
	UPROPERTY(Category = "Alarm Sight", EditAnywhere, BlueprintReadWrite)
		float SphereSize_Sight = 15.f;

	// Time-related Alarm stuff
	UPROPERTY(Category = "Alarm Time", EditAnywhere, BlueprintReadWrite)
		float AlarmMultiplier_Time;
	UPROPERTY(Category = "Alarm Time", EditAnywhere, BlueprintReadWrite)
		float AlarmReductionCooldown;
	UPROPERTY(Category = "Alarm Time", EditAnywhere, BlueprintReadWrite)
		float AlarmReductionSpeed;
	

	// Enemy speeds
	UPROPERTY(Category = "Speed", EditAnywhere, BlueprintReadWrite)
		float WalkingSpeed = 400;
	UPROPERTY(Category = "Speed", EditAnywhere, BlueprintReadWrite)
		float RunningSpeed = 600;
	UPROPERTY(Category = "Speed", EditAnywhere, BlueprintReadWrite)
		float TimeForSpeedBoost = 5.f;
	float TimeOfChase = -1.f;
	UPROPERTY(Category = "Speed", EditAnywhere, BlueprintReadWrite)
		float SpeedBoost = 800;

	FVector LastKnownTriggerPosition;
	bool CanSenseTrigger; // Name may need to be changed, the way it's used: check if the enemy can see the
						  // player

	bool IsAlarmReducing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AlarmValue;
	float TimeOfLastTrigger;
	float TimeOfFirstTrigger;
	float LastAlarmValue;

	////Patrol rotation
	//float LerpCompletion = 0;

protected:
	bool bIsPlayerVisible = false;
	bool bCanListenStimulus = true;
	float TimeRemaindingPlayer = 0.5f;
	unsigned short int SeenPoints = 0;

public:
	// BehaviorTree Tasks
	UFUNCTION(BlueprintCallable)
		virtual EPathFollowingRequestResult::Type Chase(); // The enemy chases after the player
	UFUNCTION(BlueprintCallable)
		virtual void EndReturning(); // The enemy finishes fully returning to normal behaviour
	UFUNCTION(BlueprintCallable)
		virtual void TurnToTrigger(); // The enemy turns towards a trigger

	virtual void EndTurningToWaypoint() override;
	
	UFUNCTION(BlueprintCallable)
		virtual void FaceTarget(/*FVector targetPosition*/);
	//virtual void FaceNextWaypoint();

	UPROPERTY(Category = "BehaviorTree", EditAnywhere, BlueprintReadWrite)
		float InterpolationSpeedWaypointRotate = 10.f;




	float TimerUpdate(); // Updates TimeOfLastTrigger and TimeOfFirstTrigger and returns the time since last
						 // trigger
	void UpdateAlarm_Sight(AActor* Enemy, AActor* SeenActor, float TimeDiff);
	void UpdateAlarm_Hearing(AActor* Enemy, AActor* HeardActor, float TimeDiff);
	void UpdateEnemyState();

	void UpdateVisiblePoints(AActor* Enemy, ADoorsPlayer* SeenPlayer);

	// For testing
	UFUNCTION(BlueprintCallable)
		int GetVisiblePoints();


	UFUNCTION(BlueprintImplementableEvent)
		/** Event that triggers each time State changes to Normal. */
		void OnNormalState();

	UFUNCTION(BlueprintImplementableEvent)
		/** Event that triggers each time State changes to Looking. */
		void OnLookingState();

	UFUNCTION(BlueprintImplementableEvent)
		/** Event that triggers each time State changes to Looking. */
		void OnAwareState();

	UFUNCTION(BlueprintImplementableEvent)
		/** Event that triggers each time State changes to Chasing. */
		void OnChasingState();

	UFUNCTION(BlueprintImplementableEvent)
		/** Event that triggers each time State changes to Chasing. */
		void OnReturningState();



	//Interactions
	UFUNCTION(Category = "Interactables", BlueprintImplementableEvent)
		/** Event that triggers when Enemy reachs an interactable. */
		void OnInteractDoorsAI();
	void OnInteractDoorsAICpp();

	UFUNCTION(Category = "Interactables", BlueprintCallable)
		void OnDoorDetected(UObject* DoorActor, FVector DoorLocation);

protected:
	UFUNCTION(BlueprintCallable)
		virtual void SenseNearbyTriggers(
			AActor* SensedActor, FAIStimulus Stimulus); // The enemy detects the player from sight or hearing, or
														// other triggers meant to distract them
};
