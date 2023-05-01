// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTaskNode_TurnToWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Controllers/AIDoorsController.h"

EBTNodeResult::Type UBTTaskNode_TurnToWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIDoorsController* AICController = Cast<AAIDoorsController>(OwnerComp.GetOwner());
	
	if (!AICController)
	{
		return EBTNodeResult::Failed;
	}
	
	AICController->TurnToWaypoint();
	
    return EBTNodeResult::Succeeded;
}

FString UBTTaskNode_TurnToWaypoint::GetStaticDescription() const
{
    return TEXT("Task Node for turning towards the current Waypoint.");
}