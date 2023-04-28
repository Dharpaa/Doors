#include "PropDoor.h"

#include "../AI/Controllers/AIEnemyController.h"
#include "../Enemy/DoorsEnemy.h"
#include "../Player/DoorsPlayer.h"

#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Rotator.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/Object.h"

APropDoor::APropDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootCmp = CreateDefaultSubobject<USceneComponent>(TEXT("RootCmp"));
	SetRootComponent(RootCmp);

	StaticMeshCmp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshCmp"));
	StaticMeshCmp->SetupAttachment(RootCmp);

	CentrePointCmp = CreateDefaultSubobject<USceneComponent>(TEXT("CentrePointCmp"));
	CentrePointCmp->SetupAttachment(RootCmp);

	StaticMeshCmp->SetRelativeLocation(FVector(-60.0f, -10.0f, -70.0f));

};

void APropDoor::BeginPlay()
{
	Super::BeginPlay();
	
	IsOpen = IsOpenAtStart;
};

bool APropDoor::IsOpened()
{
	return IsOpen;
}

void APropDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsMoving)
	{
		WhileMoving(DeltaTime);
	}
}

void APropDoor::Interact(float OpenDuration, float CloseDuration)
{
	if (IsMoving) { return; }

	if (NeverOpening)
	{
		return;
	}
	
	ToggleDoor(OpenDuration, CloseDuration, PlayerOpenCurve, PlayerCloseCurve);
}

void APropDoor::ToggleDoor(float OpenDuration, float CloseDuration, UCurveFloat* OpenCurve, UCurveFloat* CloseCurve)
{
	// toggle bools
	if (!IsOpen)
	{
		OpenDoor(OpenDuration, OpenCurve, CloseCurve, nullptr, true);
	}
	else
	{
		CloseDoor(CloseDuration, OpenCurve, CloseCurve);
	}
};

void APropDoor::InteractByAI(ADoorsEnemy* OtherActor, float OpenDuration, float CloseDuration)
{
	if (!IsOpen)
	{
		OpenDoor(OpenDuration, EnemyOpenCurve, EnemyCloseCurve, OtherActor, false);
	}
	else
	{
		AAIEnemyController* SaveEnemyController = Cast<AAIEnemyController>(OtherActor->GetController());
		if (SaveEnemyController)
		{
			if (SaveEnemyController->CurrentState != EnemyStates::Chasing && SaveEnemyController->CurrentState != EnemyStates::Looking)
			{
				CloseDoor(CloseDuration, EnemyOpenCurve, EnemyCloseCurve);
			}
		}
	}
}

void APropDoor::OpenDoor(float Duration, UCurveFloat* OpenCurve, UCurveFloat* CloseCurve, ADoorsEnemy* OtherActor, bool bIsPlayer) {

	if (IsMoving) { return; }
	FVector Direction = FVector::ZeroVector;
	if (bIsPlayer)
	{
		FVector PlayerDir = ADoorsPlayer::DoorsPlayer->GetActorLocation() - CentrePointCmp->GetComponentLocation();
		Direction = UKismetMathLibrary::LessLess_VectorRotator(PlayerDir, GetActorRotation());
	}
	else
	{
		FVector EnemyDir = OtherActor->GetActorLocation() - CentrePointCmp->GetComponentLocation();
		Direction = UKismetMathLibrary::LessLess_VectorRotator(EnemyDir, GetActorRotation());
	}

	if (Direction.Y > 0)
	{
		EndRotation = FRotator(0.0f, -OpenCloseAngle, 0.0f);
	}
	else
	{
		EndRotation = FRotator(0.0f, OpenCloseAngle, 0.0f);
	}

	StartRotation = FRotator::ZeroRotator;
	IsOpen = true;
	IsMoving = true;
	MoveTimer = 0.0f;
	MoveDuration = Duration;
	MoveOpenCurve = OpenCurve;
	MoveCloseCurve = CloseCurve;
	bHasMoved = true;
};

void APropDoor::CloseDoor(float Duration, UCurveFloat* OpenCurve, UCurveFloat* CloseCurve) {

	if (IsMoving) { return; }

	StartRotation = StaticMeshCmp->GetRelativeRotation();
	EndRotation = FRotator::ZeroRotator;

	IsOpen = false;
	IsMoving = true;
	MoveTimer = 0.0f;
	MoveDuration = Duration;
	MoveOpenCurve = OpenCurve;
	MoveCloseCurve = CloseCurve;
	bHasMoved = true;
};

void APropDoor::WhileMoving(float DeltaTime) {

	if (MoveTimer < MoveDuration)
	{
		auto Completion = MoveTimer / MoveDuration;
		auto Lerp = FMath::Lerp(StartRotation, EndRotation, Completion);
		Lerp = Lerp.Clamp();
		if (MoveOpenCurve && MoveCloseCurve)
		{
			if (!IsOpen) {
				Lerp *= MoveCloseCurve->GetFloatValue(Completion);
			}
			else
			{
				Lerp *= MoveOpenCurve->GetFloatValue(Completion);
			}
		}
		StaticMeshCmp->SetRelativeRotation(Lerp);
		MoveTimer += DeltaTime;
	}
	else
	{
		if (!IsOpen)
		{
			StaticMeshCmp->SetRelativeRotation(FRotator::ZeroRotator);
		}
		else
		{
			StaticMeshCmp->SetRelativeRotation(EndRotation);
		}
		MoveTimer = 0.0f;
		MoveDuration = 0.0f;
		IsMoving = false;
		MoveCloseCurve = nullptr;
		MoveOpenCurve = nullptr;
	}
};
