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

	// Set variables
	IsAlarmReducing = false;
	AlarmValue = 0;
	TimeOfFirstTrigger = -1;
	TimeOfLastTrigger = -1;

	SetUpPerception();
}

void AAIEnemyController::BeginPlay()
{
	Super::BeginPlay();
	SetUpPerceptionValues();

	PlayerRef = ADoorsPlayer::DoorsPlayer;
}

void AAIEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsPlayerVisible)
	{
		if (TimeRemaindingPlayer > 0)
		{
			TimeRemaindingPlayer -= DeltaTime;

			if (TimeRemaindingPlayer <= 0.01f)
			{
				bCanListenStimulus = true;
				if (BlackboardComponent)
				{
					BlackboardComponent->SetValueAsBool(DoorsBlackboardKeys::canSearchInteractables, true);
				}
				TimeRemaindingPlayer = 0;
			}
		}
	}
}

void AAIEnemyController::SetUpPerceptionValues()
{
	if (SightConfig)
	{
		// Configuring the Sight Sense
		SightConfig->SightRadius = VisionRadiusDetect;
		SightConfig->LoseSightRadius = VisionRadiusLose;
		SightConfig->PeripheralVisionAngleDegrees = VisionPeriphericalRadius;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		AIPerceptionComponent->ConfigureSense(*SightConfig);
		AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	}

	if (HearingConfig)
	{
		// Configuring the Hearing Sense
		HearingConfig->HearingRange = HearRadius;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

		// Asigning Senses to the AI PerceptionComponent
		AIPerceptionComponent->ConfigureSense(*HearingConfig);
	}
}

void AAIEnemyController::SetUpPerception()
{
	// Create AI PerceptionCMP
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("MskPerceptionComponent"));

	// Creates the Senses
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("Sight Config"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(FName("Hearing Config"));

	SetUpPerceptionValues();
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
		this, &AAIEnemyController::OnTargetPerceptionUpdate);
	// MskPerceptionComponent->OnPerceptionUpdated.AddDynamic(this,
	// &AAIEnemyController::OnAllPerceptionUpdate);
}

void AAIEnemyController::SetPerceptionToZero()
{
	if (SightConfig)
	{
		// Configuring the Sight Sense
		SightConfig->SightRadius = 0;
		SightConfig->LoseSightRadius = 0;
		SightConfig->PeripheralVisionAngleDegrees = 0;

		AIPerceptionComponent->ConfigureSense(*SightConfig);
		AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	}

	if (HearingConfig)
	{
		// Configuring the Hearing Sense
		HearingConfig->HearingRange = 0;

		// Asigning Senses to the AI PerceptionComponent
		AIPerceptionComponent->ConfigureSense(*HearingConfig);
	}

}

void AAIEnemyController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	// Sound Stimuli
	if (Stimulus.Tag == DoorsAItags::noise_tag)
	{

		ADoorsPlayer* HeardPlayer = Cast<ADoorsPlayer>(Actor);
		ADoorsEnemy* enemy = Cast<ADoorsEnemy>(GetPawn());
		if (enemy && HeardPlayer && HeardPlayer->GetState() != PlayerStateEnum::SNEAK_WALK &&
			(FMath::Abs(HeardPlayer->GetTargetLocation().Z - GetPawn()->GetTargetLocation().Z) < enemy->fHeightDistanceHear))
		{
			SenseNearbyTriggers(Actor, Stimulus);
		}
	}
	else if (Stimulus.Tag == DoorsAItags::noiseinteractable_tag)
	{
		if (bCanListenStimulus)
		{
			AlarmValue = AlarmLevel_Chasing - 1;

			BlackboardComponent->SetValueAsObject(DoorsBlackboardKeys::interactibleObject, Actor);
			BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::interactibleLocation,
				Actor->GetActorLocation());

			SenseNearbyTriggers(Actor, Stimulus);
		}
	}
	// Visual Stimuli
	else if (Stimulus.Type.Name == "Default__AISense_Sight")
	{
		bool isEntered = Stimulus.WasSuccessfullySensed();

		if (Cast<ADoorsPlayer>(Actor))
		{
			// Get a Reference for Player
			if (!PlayerRef)
			{
				PlayerRef = Cast<ADoorsPlayer>(Actor);
			}
			CanSenseTrigger = isEntered;
			bIsPlayerVisible = isEntered; // For timer to check when player is visible
			
			// Blackboard Set Var
			BlackboardComponent->SetValueAsBool(DoorsBlackboardKeys::isPlayerVisible, isEntered);

			if (isEntered)
			{
				SenseNearbyTriggers(Actor, Stimulus);

				BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::playerLocation,
					Actor->GetActorLocation());
				if (BlackboardComponent->GetValueAsObject(DoorsBlackboardKeys::playerObj) == NULL)
				{
					BlackboardComponent->SetValueAsObject(DoorsBlackboardKeys::playerObj, PlayerRef);
				}
			}
			else
			{
				LastKnownTriggerPosition = Actor->GetActorLocation();
				BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::lastKnownLocation,
					LastKnownTriggerPosition);

				BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::playerLocation,
					Actor->GetActorLocation());
				if (BlackboardComponent->GetValueAsObject(DoorsBlackboardKeys::playerObj) == NULL)
				{
					BlackboardComponent->SetValueAsObject(DoorsBlackboardKeys::playerObj, PlayerRef);
				}
			}
		}
	}
}

float AAIEnemyController::TimerUpdate()
{

	float TimeDiff;
	// If the enemy is untriggered, this is the "first" trigger
	if (TimeOfFirstTrigger == -1)
	{
		TimeOfFirstTrigger = TimeOfLastTrigger;
	}

	if (TimeOfLastTrigger == -1)
	{
		TimeDiff = 0;
		TimeOfLastTrigger = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	}
	else
	{
		// If we want an exponential time alarm modifier
		TimeOfLastTrigger = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
		TimeDiff = TimeOfLastTrigger - TimeOfFirstTrigger;

		// If we want a lineal time alarm modifier
		//TimeDiff = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) - TimeOfLastTrigger;
		//TimeOfLastTrigger = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	}

	return TimeDiff;
}

void AAIEnemyController::UpdateAlarm_Sight(AActor* Enemy, AActor* SeenActor, float TimeDiff)
{
	ADoorsPlayer* SeenPlayer = Cast<ADoorsPlayer>(SeenActor);
	if (!SeenPlayer)
	{
		return;
	}
	
	FVector EnemyPlayerVector = SeenActor->GetTargetLocation() - Enemy->GetTargetLocation();
	float EnemyPlayerDistance = EnemyPlayerVector.Size();
	float PlayerStateMultiplier = 1;

	EnemyPlayerVector.Normalize();
	UpdateVisiblePoints(Enemy, SeenPlayer);

	float SightAngle = FMath::RadiansToDegrees(
		FMath::Acos(FVector::DotProduct(Enemy->GetActorForwardVector(), EnemyPlayerVector)));
	if (SightAngle > 90)
	{
		SightAngle = 90;
	}

	if (SeenPlayer->GetState() == PlayerStateEnum::WANDER_RUN)
	{
		PlayerStateMultiplier = AlarmMultiplier_PlayerRunningSight;
	}
	else if (SeenPlayer->GetState() == PlayerStateEnum::SNEAK_IDLE ||
		SeenPlayer->GetState() == PlayerStateEnum::SNEAK_WALK)
	{
		PlayerStateMultiplier = AlarmMultiplier_PlayerSneakingSight;
	}
	else
	{
		PlayerStateMultiplier = AlarmMultiplier_PlayerWalkingSight;
	}

	// If the player is in the LoseSightRadius, we only add the time
	if (SightConfig->SightRadius - EnemyPlayerDistance < 0)
	{
		AlarmValue += TimeDiff * AlarmMultiplier_Time;
	}
	else
	{
		AlarmValue +=
			AlarmMultiplier_Factor *
			(AlarmMultiplier_DistanceSight * (SightConfig->SightRadius - EnemyPlayerDistance) /
				SightConfig->SightRadius) *
			(AlarmMultiplier_AngleSight * (SightConfig->PeripheralVisionAngleDegrees - SightAngle) /
				(SightConfig->PeripheralVisionAngleDegrees)) *
			PlayerStateMultiplier * (AlarmMultiplier_PointsSight * SeenPoints / 4.f) +
			(TimeDiff * AlarmMultiplier_Time);
	}

	if (AlarmValue > AlarmMaximumValue)
	{
		AlarmValue = AlarmMaximumValue;
	}

	BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::lastKnownLocation,
		SeenPlayer->GetActorLocation());
	BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::playerLocation,
		SeenPlayer->GetActorLocation());
	if (BlackboardComponent->GetValueAsObject(DoorsBlackboardKeys::playerObj) == NULL)
	{
		BlackboardComponent->SetValueAsObject(DoorsBlackboardKeys::playerObj, PlayerRef);
	}
	
}

void AAIEnemyController::UpdateAlarm_Hearing(AActor* Enemy, AActor* HeardActor, float TimeDiff)
{

	ADoorsPlayer* HeardPlayer = Cast<ADoorsPlayer>(HeardActor);
	if (!HeardPlayer)
	{
		return;
	}

	FVector EnemyPlayerVector = Enemy->GetTargetLocation() - HeardActor->GetTargetLocation();
	float EnemyPlayerDistance = EnemyPlayerVector.Size();
	float PlayerStateMultiplier = 1;
	bool IsSoundLow = false;



	if (HeardPlayer->GetState() == PlayerStateEnum::WANDER_RUN)
	{
		PlayerStateMultiplier = AlarmMultiplier_PlayerRunningHearing;
	}
	else
	{
		PlayerStateMultiplier = AlarmMultiplier_PlayerWalkingHearing;
		IsSoundLow = true;
	}

	//Attenuation walls
	float fAttenuationWalls = 1.f;
	int currentWallsThrough = 0;
	auto Result = TArray<FHitResult>();
	auto* World = GetWorld();
	auto Collided = false;
	auto Start = FVector::ZeroVector;
	auto End = FVector::ZeroVector;


	Start = Enemy->GetTargetLocation();
	End = HeardActor->GetTargetLocation();

	bool bSoundHitted = UKismetSystemLibrary::LineTraceMulti(
		World, Start, End, ETraceTypeQuery::TraceTypeQuery1, false,
		{ ADoorsPlayer::DoorsPlayer }, EDrawDebugTrace::ForOneFrame, Result, true);

	if (bSoundHitted)
	{
		for (auto& Hit : Result)
		{
			++iMaxWallsToStopHearing;
			fAttenuationWalls *= 0.5f; //Sound strenght halved for each wall hit

			if (Hit.Distance >= fMaxDistanceAttenuationWall || currentWallsThrough >= iMaxWallsToStopHearing)
			{
				fAttenuationWalls = 0.f;
				break;
			}
		}
	}
	
	AlarmValue += AlarmMultiplier_Factor *
		fAttenuationWalls *
		(AlarmMultiplier_DistanceHearing * (HearingConfig->HearingRange - EnemyPlayerDistance) /
			HearingConfig->HearingRange) *
		PlayerStateMultiplier +
		(TimeDiff * AlarmMultiplier_Time);

	if (fAttenuationWalls <= 0.3f)
	{
		if (IsSoundLow && CurrentState == EnemyStates::Looking && AlarmValue > AlarmLevel_Chasing)
		{
			AlarmValue = AlarmLevel_Chasing - 1;
		}
	}

	if (AlarmValue > AlarmMaximumValue)
	{
		AlarmValue = AlarmMaximumValue;
	}
}

void AAIEnemyController::SenseNearbyTriggers(AActor* SensedActor, FAIStimulus Stimulus)
{

	// If the alarm value was being reduced, it stops
	IsAlarmReducing = false;

	AActor* Enemy = GetPawn();
	LastKnownTriggerPosition = Stimulus.StimulusLocation;

	float TimeDiff = TimerUpdate();

	// For ease of use later, only one of these can be True
	bool IsSenseHearing = (UAISense::GetSenseID<UAISense_Hearing>().Index == Stimulus.Type.Index);
	bool IsSenseSight = (UAISense::GetSenseID<UAISense_Sight>().Index == Stimulus.Type.Index);
	bool IsSenseTouch = (UAISense::GetSenseID<UAISense_Touch>().Index == Stimulus.Type.Index);

	// If the enemy was Returning and interrupted, it returns to the value from the latest trigger
	if (CurrentState == EnemyStates::Returning)
	{
		AlarmValue = LastAlarmValue;
		// This State is simply meant to be replaced further below
		CurrentState = EnemyStates::Normal;
	}

	ADoorsPlayer* SensedPlayer = Cast<ADoorsPlayer>(SensedActor);
	if (SensedPlayer && Enemy)
	{
		if (IsSenseTouch)
		{
			// Directly to level Chasing, maximum value even
			if (AlarmValue < AlarmMaximumValue)
			{
				AlarmValue = AlarmMaximumValue - 1;
			}
		}
		else if (IsSenseSight)
		{
			bCanListenStimulus = false;
			TimeRemaindingPlayer = TimeRemaindingPlayer_Max;
			if (BlackboardComponent)
			{
				BlackboardComponent->SetValueAsBool(DoorsBlackboardKeys::canSearchInteractables, false);
			}
			UpdateAlarm_Sight(Enemy, SensedActor, TimeDiff);
		}
		else if (IsSenseHearing)
		{
			UpdateAlarm_Hearing(Enemy, SensedActor, TimeDiff);
		}
	}
	
	UpdateEnemyState();

	BlackboardComponent->SetValueAsVector(DoorsBlackboardKeys::lastKnownLocation, LastKnownTriggerPosition); 
}

EPathFollowingRequestResult::Type AAIEnemyController::Chase()
{
	ADoorsPlayer* Player = Cast<ADoorsPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ADoorsPlayer::StaticClass()));

	if (!Player)
	{
		return EPathFollowingRequestResult::Failed;
	}

	EPathFollowingRequestResult::Type MoveToPlayerResult = MoveToActor(Player);
	return MoveToPlayerResult;
}

void AAIEnemyController::UpdateEnemyState()
{
	// Alarm Level can change the State upwards, but never downwards here
	switch (CurrentState)
	{
		case EnemyStates::Normal: 
			if (AlarmValue >= AlarmLevel_Aware)
			{
				if (AlarmValue >= AlarmLevel_Looking)
				{
					if (AlarmValue >= AlarmLevel_Chasing)
					{
						CurrentState = EnemyStates::Chasing;
						if (GetPawn() && Cast<ADoorsEnemy>(GetPawn()))
						{
							Cast<ADoorsEnemy>(GetPawn())->GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
						}
						if (TimeOfChase == -1.f)
						{
							TimeOfChase = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
						}
						OnChasingState();
					}
					else
					{
						CurrentState = EnemyStates::Looking;
						OnLookingState();
					}
				}
				else
				{
					CurrentState = EnemyStates::Aware;
					OnAwareState();
				}
			}
			break;
		
		
		case EnemyStates::Aware: 
			if (AlarmValue >= AlarmLevel_Looking)
			{
				if (AlarmValue >= AlarmLevel_Chasing)
				{
					CurrentState = EnemyStates::Chasing;
					if (GetPawn() && Cast<ADoorsEnemy>(GetPawn()))
					{
						Cast<ADoorsEnemy>(GetPawn())->GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
					}
					if (TimeOfChase == -1.f)
					{
						TimeOfChase = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
					}
					OnChasingState();
				}
				else
				{
					CurrentState = EnemyStates::Looking;
					OnLookingState();
				}
			}
			break;
		
		case EnemyStates::Looking: 
			if (AlarmValue >= AlarmLevel_Chasing)
			{
				CurrentState = EnemyStates::Chasing;
				if (GetPawn() && Cast<ADoorsEnemy>(GetPawn()))
				{
					Cast<ADoorsEnemy>(GetPawn())->GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
				}
				if (TimeOfChase == -1.f)
				{
					TimeOfChase = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
				}
				OnChasingState();
			}
			break;
		
		case EnemyStates::Chasing: 
			break;
		
	}

	BlackboardComponent->SetValueAsEnum(DoorsBlackboardKeys::currentState, (uint8)CurrentState);
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

	FVector HeadLocation;
	FVector HeadDirection;
	this->PerceptionComponent->GetLocationAndDirection(HeadLocation, HeadDirection);

	FCollisionShape VisionSphere = FCollisionShape::MakeSphere(SphereSize_Sight);

	// Checking for each arm
	for (int i = 0; i < Sockets.Num(); i++)
	{
		FVector SocketLocation = Sockets[i]->GetSocketLocation(ArmsMeshComponent);
		GetWorld()->SweepSingleByChannel(HitResult, HeadLocation, SocketLocation,
			FQuat::Identity, ECC_Visibility, VisionSphere,
			FCollisionQueryParams(NAME_AILineOfSight, false, this));

		if (Cast<ADoorsPlayer>(HitResult.Actor) != nullptr)
		{
			SeenPoints++;
		}
	}

	// Checking for the head
	FVector AuxCameraLocation = SeenPlayer->GetCameraCmp()->GetComponentLocation();
	GetWorld()->SweepSingleByChannel(HitResult, HeadLocation, AuxCameraLocation, FQuat::Identity,
		ECC_Visibility, VisionSphere,
		FCollisionQueryParams(NAME_AILineOfSight, false, this));
	if (Cast<ADoorsPlayer>(HitResult.Actor) != nullptr)
	{
		SeenPoints++;
	}

	// Checking for the chest
	FVector AuxPlayerLocation = SeenPlayer->GetTargetLocation();
	GetWorld()->SweepSingleByChannel(HitResult, HeadLocation, AuxPlayerLocation, FQuat::Identity,
		ECC_Visibility, VisionSphere,
		FCollisionQueryParams(NAME_AILineOfSight, false, this));
	if (Cast<ADoorsPlayer>(HitResult.Actor) != nullptr)
	{
		SeenPoints++;
	}
}

void AAIEnemyController::EndReturning()
{
	OnNormalState();
	CurrentState = EnemyStates::Normal;
	BlackboardComponent->SetValueAsEnum(DoorsBlackboardKeys::currentState, (uint8)EnemyStates::Normal);
	TimeOfFirstTrigger = -1;
	TimeOfLastTrigger = -1;
	TimeOfChase = -1;
}

void AAIEnemyController::OnInteractDoorsAICpp()
{
	OnInteractDoorsAI();
}

void AAIEnemyController::TurnToTrigger()
{
	APawn* EnemyPawn = GetPawn();
	if (EnemyPawn)
	{
		FRotator NewRotation = FRotationMatrix::MakeFromX(LastKnownTriggerPosition - EnemyPawn->GetActorLocation()).Rotator();
		FRotator Interpolation = FMath::RInterpTo(ControlRotation, NewRotation, GetWorld()->GetDeltaSeconds(), 1);
		EnemyPawn->SetActorRotation(Interpolation);
	}
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

