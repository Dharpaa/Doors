#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIDoorsController.generated.h"

UCLASS()
class AAIDoorsController : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere)
	UBlackboardComponent* BlackboardComponent;

private:
	UPROPERTY(SaveGame)
	TArray<int> UndiscoveredIndexes;

	UPROPERTY(SaveGame)
		int NextIndex = 0;

	
public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;

	UFUNCTION(BluePrintCallable)
	void UpdateNextTargetPoint(); //Indicates where the npc should go
	
	UFUNCTION(BlueprintCallable)
	virtual void TurnToWaypoint();

	virtual void EndTurningToWaypoint();
	//Patrol rotation
	float LerpCompletion = 0;
	
	virtual EPathFollowingRequestResult::Type Patrol();
};
