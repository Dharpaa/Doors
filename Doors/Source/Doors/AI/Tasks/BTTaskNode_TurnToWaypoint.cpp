// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTaskNode_TurnToWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Controllers/AIDoorsController.h"

EBTNodeResult::Type UBTTaskNode_TurnToWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIDoorsController* AICMskController = Cast<AAIDoorsController>(OwnerComp.GetOwner());
	
	if (!AICMskController)
	{
		return EBTNodeResult::Failed;
	}
	
	AICMskController->TurnToWaypoint();
	
    return EBTNodeResult::Succeeded;
}

FString UBTTaskNode_TurnToWaypoint::GetStaticDescription() const
{
    return TEXT("Task Node for turning towards the current Waypoint.");
}