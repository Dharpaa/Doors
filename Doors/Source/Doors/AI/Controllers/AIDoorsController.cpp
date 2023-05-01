#include "AIDoorsController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "../DoorsTargetPoint.h"
#include "BrainComponent.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include <stdlib.h> 
#include "../../Logger.h"
#include "Doors/AI/DoorsAITag.h"
#include "Doors/Enemy/DoorsEnemy.h"

void AAIDoorsController::OnPossess(class APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (BehaviorTree != nullptr) {
		RunBehaviorTree(BehaviorTree);
	}
	BlackboardComponent = BrainComponent->GetBlackboardComponent();

	ADoorsEnemy* EnemyActor = Cast<ADoorsEnemy>(GetPawn());
	// If WaypointIndex is -1, the npc is "turret" and doesn't move
	if (EnemyActor->WayPoints.Num() == 0)
	{
		BlackboardComponent->SetValueAsInt(DoorsBlackboardKeys::waypointIndex, 0);
		BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::waypointLocation, EnemyActor->GetTargetLocation());
	}
}

void AAIDoorsController::BeginPlay()
{
	Super::BeginPlay();
}

void AAIDoorsController::UpdateNextTargetPoint()
{
	int32 TargetPointIndex = BlackboardComponent->GetValueAsInt(DoorsBlackboardKeys::waypointIndex);

	ADoorsEnemy* EnemyActor = Cast<ADoorsEnemy>(GetPawn());
	TArray<ADoorsTargetPoint*> Waypoints = EnemyActor->WayPoints;

	unsigned int WaypointsLen = Waypoints.Num();
	unsigned int NewTargetPointIndex;

	// If the NPC is just a turret, no need to update
	if (WaypointsLen == 0)
	{
		return;
	}

	if (EnemyActor->PatrolInOrder)
	{
		NewTargetPointIndex = (TargetPointIndex + 1) % WaypointsLen;
	}
	else
	{
		if (UndiscoveredIndexes.Num() == 0)
		{
			for (unsigned int i = 0; i < WaypointsLen; i++)
			{
				UndiscoveredIndexes.Add(i);
			}
		}
		unsigned int AuxIndex = rand() % UndiscoveredIndexes.Num();
		NewTargetPointIndex = UndiscoveredIndexes.GetData()[AuxIndex];
		UndiscoveredIndexes.RemoveAt(AuxIndex);
	}

	NextIndex = NewTargetPointIndex;
	Logger::Log(false, NextIndex);
	BlackboardComponent->SetValueAsInt(DoorsBlackboardKeys::waypointIndex, NewTargetPointIndex);
	BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::waypointLocation , Waypoints.GetData()[NewTargetPointIndex]->GetTargetLocation());
	BlackboardComponent->SetValueAsRotator(DoorsBlackboardKeys::waypointRotation, Waypoints.GetData()[NewTargetPointIndex]->GetActorRotation());
	BlackboardComponent->SetValueAsFloat(DoorsBlackboardKeys::waypointWaitTime, Waypoints.GetData()[NewTargetPointIndex]->GetWaitTime());
	BlackboardComponent->SetValueAsBool(DoorsBlackboardKeys::notFacingWayPoint, true);
	BlackboardComponent->SetValueAsBool("NotFacingTarget", true);
	BlackboardComponent->SetValueAsObject(DoorsBlackboardKeys::waypointObject, Waypoints.GetData()[NewTargetPointIndex]);
}


void AAIDoorsController::TurnToWaypoint()
{
	APawn* EnemyPawn = GetPawn();
	if (EnemyPawn)
	{
		auto Lerp = FMath::Lerp(EnemyPawn->GetActorRotation(),
			BlackboardComponent->GetValueAsRotator(DoorsBlackboardKeys::waypointRotation),
			LerpCompletion);
		EnemyPawn->SetActorRotation(Lerp);
		LerpCompletion += 0.01;
		if (BlackboardComponent->GetValueAsRotator(DoorsBlackboardKeys::waypointRotation)
			.Equals(EnemyPawn->GetActorRotation()))
		{
			EndTurningToWaypoint();
		}
	}
}

void AAIDoorsController::EndTurningToWaypoint()
{
	LerpCompletion = 0;
	BlackboardComponent->SetValueAsBool(DoorsBlackboardKeys::notFacingWayPoint, false);
}

EPathFollowingRequestResult::Type AAIDoorsController::Patrol()
{

	FVector Waypoint = BlackboardComponent->GetValueAsVector(DoorsBlackboardKeys::waypointLocation);

	EPathFollowingRequestResult::Type MoveToTargetResult = MoveToLocation(Waypoint);

	return MoveToTargetResult;
}

