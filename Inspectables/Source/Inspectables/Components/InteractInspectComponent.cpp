#include "InteractInspectComponent.h"
#include "../Player/InspectPlayer.h"
#include "Camera/CameraComponent.h"
#include "Datasmith/DatasmithCore/Public/DatasmithDefinitions.h"
#include "Kismet/KismetMathLibrary.h"


UInteractInspectComponent::UInteractInspectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	InspectMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InspectMesh"));
	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight1"));
	PointLightComponent->SetLightingChannels(false,true,false);
	
}

void UInteractInspectComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractInspectComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	
	InspectMeshComponent->SetupAttachment(this);
	
	PointLightComponent->SetupAttachment(this);
	PointLightComponent->SetWorldLocation(this->GetOwner()->GetActorLocation());
	PointLightComponent->SetAbsolute(true,true,false);

}

void UInteractInspectComponent::Inspect()
{
	if (AInspectPlayer::Player->GetState() == PlayerStateEnum::INSPECTING)
	{
		return;
	}
	
	PointLightComponent->SetWorldLocation(AInspectPlayer::Player->GetCameraCmp()->GetComponentLocation());
	InspectMeshComponent->SetLightingChannels(false, true, false);
	InspectCanceled = false;
	OldInspectableTransform = this->GetRelativeTransform();
	
	AInspectPlayer::Player->LookAt(this->GetOwner()->GetActorLocation());

	AInspectPlayer::Player->SetState(PlayerStateEnum::INSPECTING, true);
	SetComponentTickEnabled(true);
}

void UInteractInspectComponent::CancelInspect()
{
	this->SetRelativeTransform(OldInspectableTransform);
	InspectCanceled = true;
	SetComponentTickEnabled(false);
	InspectMeshComponent->SetLightingChannels(true, false, false);
	OnInspectCancel.Broadcast();

	OldInspectableTransform = FTransform::Identity;
	DeltaLookAxis = FVector2D::ZeroVector;
}

void UInteractInspectComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction *ThisTickFunction)
{
	if (AInspectPlayer::Player->GetState() == PlayerStateEnum::INSPECTING)
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
		DeltaLookAxis = DeltaLookAxis + AInspectPlayer::Player->GetLookAxis();	// Get the difference between the current and previous look axis
		
		//Roll
		FQuat PitchQuat = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(-DeltaLookAxis.Y * DeltaTime * (RotationSpeed * 10 )));
		//Pitch
		FQuat YawQuat = FQuat(FVector(0, 0, 1), FMath::DegreesToRadians(-DeltaLookAxis.X * DeltaTime * (RotationSpeed * 10 )));

		FQuat _ActorRotation = FQuat::Identity;
		_ActorRotation = _ActorRotation * PitchQuat * YawQuat;
		_ActorRotation.Normalize();
		this->AddWorldRotation(_ActorRotation);
		PrevLookAxis = DeltaLookAxis;
		DeltaLookAxis = FVector2D::ZeroVector;
	}
}
