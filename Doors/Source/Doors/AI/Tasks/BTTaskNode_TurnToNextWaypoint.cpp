// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TurnToNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Controllers/AIEnemyController.h"

EBTNodeResult::Type UBTTaskNode_TurnToNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIEnemyController* AIEnemyController = Cast<AAIEnemyController>(OwnerComp.GetOwner());

    if (!AIEnemyController) return EBTNodeResult::Failed;

    AIEnemyController->FaceTarget();

    return EBTNodeResult::Succeeded;
}

FString UBTTaskNode_TurnToNextWaypoint::GetStaticDescription() const
{
    return TEXT("Task Node for turning towards the Next Waypoint.");
}