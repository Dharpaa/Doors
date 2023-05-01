#include "BTTaskNode_UpdateNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Controllers/AIDoorsController.h"

EBTNodeResult::Type UBTTaskNode_UpdateNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIDoorsController* AIController = Cast<AAIDoorsController>(OwnerComp.GetOwner());

    AIController->UpdateNextTargetPoint();

    return EBTNodeResult::Succeeded;
}


FString UBTTaskNode_UpdateNextWaypoint::GetStaticDescription() const
{
    return TEXT("Task Node for updating to a new Waypoint in the NPCs route.");
}