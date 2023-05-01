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

public:
	AAIEnemyController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EnemyStates CurrentState = EnemyStates::Normal;
	
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

protected:
	bool bIsPlayerVisible = false;
	unsigned short int SeenPoints = 0;

public:
	// BehaviorTree Tasks
	UFUNCTION(BlueprintCallable)
		virtual void EndReturning(); // The enemy finishes fully returning to normal behaviour

	virtual void EndTurningToWaypoint() override;
	
	UFUNCTION(BlueprintCallable)
		virtual void FaceTarget();

	UPROPERTY(Category = "BehaviorTree", EditAnywhere, BlueprintReadWrite)
		float InterpolationSpeedWaypointRotate = 10.f;
	

	void UpdateVisiblePoints(AActor* Enemy, ADoorsPlayer* SeenPlayer);

	// For testing
	UFUNCTION(BlueprintCallable)
		int GetVisiblePoints();


	//Interactions
	UFUNCTION(Category = "Interactables", BlueprintImplementableEvent)
	/** Event that triggers when Enemy reachs an interactable. */
	void OnInteractDoorsAI();
	void OnInteractDoorsAICpp();

	UFUNCTION(Category = "Interactables", BlueprintCallable)
		void OnDoorDetected(UObject* DoorActor, FVector DoorLocation);
};
