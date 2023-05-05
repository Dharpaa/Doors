#include "AIEnemyController.h"
#include "../DoorsAITag.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "../../Player/DoorsPlayer.h"
#include "../../Enemy/DoorsEnemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Touch.h"
#include "GameFramework/CharacterMovementComponent.h"

AAIEnemyController::AAIEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAIEnemyController::BeginPlay()
{
	Super::BeginPlay();
}

void AAIEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAIEnemyController::UpdateVisiblePoints(AActor* Enemy, ADoorsPlayer* SeenPlayer)
{
	static const FName NAME_AILineOfSight = FName(TEXT("AICheckSeePoints"));
	SeenPoints = 0;
	FHitResult HitResult;
	USkeletalMeshComponent* ArmsMeshComponent = Cast<USkeletalMeshComponent>(SeenPlayer->GetCameraCmp()->GetChildComponent(4));

	if (!ArmsMeshComponent)
	{
		return;
	}
	auto Sockets = ArmsMeshComponent->SkeletalMesh->GetActiveSocketList();
}

void AAIEnemyController::EndReturning()
{
	CurrentState = EnemyStates::Normal;
	BlackboardComponent->SetValueAsEnum(DoorsBlackboardKeys::currentState, (uint8)EnemyStates::Normal);
	TimeOfChase = -1;
}

void AAIEnemyController::OnInteractDoorsAICpp()
{
	OnInteractDoorsAI();
}

void AAIEnemyController::EndTurningToWaypoint()
{
	Super::EndTurningToWaypoint();
}

void AAIEnemyController::FaceTarget()
{
	APawn* EnemyPawn = GetPawn();
	if (EnemyPawn)
	{
		FVector vWaypointLocation =
			BlackboardComponent->GetValueAsVector(DoorsBlackboardKeys::waypointLocation);
		FVector vEnemyLoc = EnemyPawn->GetActorLocation();
		FVector fromEnemyToTarget = (vWaypointLocation - vEnemyLoc);
		if (fromEnemyToTarget.Size() > 200.f)
			// if (fromEnemyToTarget.SizeSquared() > 200.f*200.f)
		{
			float size = fromEnemyToTarget.SizeSquared();
			FVector fromEnemyToTargetNormalize = fromEnemyToTarget / fromEnemyToTarget.Size();
			float vectorProduct =
				FVector::DotProduct(fromEnemyToTargetNormalize, (EnemyPawn->GetActorForwardVector()));

			auto Lerp = FMath::Lerp(EnemyPawn->GetActorRotation(), fromEnemyToTargetNormalize.Rotation(),
				LerpCompletion);
			EnemyPawn->SetActorRotation(Lerp);
			LerpCompletion += 0.01;
			if (LerpCompletion >= 1)
			{
				BlackboardComponent->SetValueAsBool("NotFacingTarget", false);
				LerpCompletion = 0;
			}
		}
		else
		{
			BlackboardComponent->SetValueAsBool("NotFacingTarget", false);
			LerpCompletion = 0;
		}
	}
}

void AAIEnemyController::OnDoorDetected(UObject* DoorActor, FVector DoorLocation)
{
	BlackboardComponent->SetValueAsObject(DoorsBlackboardKeys::interactibleObject, DoorActor);
	BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::interactibleLocation, DoorLocation);
}

int AAIEnemyController::GetVisiblePoints()
{
	return SeenPoints;
}

