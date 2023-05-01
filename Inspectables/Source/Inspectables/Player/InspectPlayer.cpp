#include "InspectPlayer.h"
#include "InspectPlayerController.h"
#include "States/PlayerStateBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/AISense_Hearing.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "../GameMode/InspectGameplayGameMode.h"
#include "../Interfaces/InteractableInterface.h"
#include "../Interfaces/OutlinableInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../Logger.h"

#define IsImplemented UKismetSystemLibrary::DoesImplementInterface

// Base

AInspectPlayer *AInspectPlayer::Player = nullptr;

AInspectPlayer::AInspectPlayer()
{
    Player = this;

    PrimaryActorTick.bCanEverTick = true;

    // Components
    SetRootComponent((USceneComponent *)GetCapsuleComponent());

    SpringArmCmp = CreateDefaultSubobject<USpringArmComponent>("SpringArmCmp");
    SpringArmCmp->SetupAttachment(RootComponent);
    SpringArmCmp->bUsePawnControlRotation = true;

    CameraCmp = CreateDefaultSubobject<UCameraComponent>("CameraCmp");
    CameraCmp->SetupAttachment(SpringArmCmp, USpringArmComponent::SocketName);

    HandSocketCmp = CreateDefaultSubobject<USceneComponent>("HandSocketCmp");
    HandSocketCmp->SetupAttachment(SpringArmCmp);

    // Set eye position
    SpringArmCmp->SetRelativeLocation({0.f, 0.f, BaseEyeHeight});
}

void AInspectPlayer::BeginPlay()
{
    Super::BeginPlay();

    auto *World = GetWorld();

    // Log reserve space
    Logger::Log(ShowDebugStateChanges, "-", 5.f, FColor::Transparent, 0);
    Logger::Log(ShowDebugInteractables, "-", 5.f, FColor::Transparent, 1);
    Logger::Log(ShowDebugTap, "-", 5.f, FColor::Transparent, 2);
    Logger::Log(ShowDebugTap, "-", 5.f, FColor::Transparent, 3);

    // Cache PlayerController
    PlayerController = Cast<AInspectPlayerController>(GetController());
    if (PlayerController)
        PlayerCameraManager = PlayerController->PlayerCameraManager;

    // Cache GameMode
    GameMode = Cast<AInspectGameplayGameMode>(UGameplayStatics::GetGameMode(World));
    Logger::Log(!GameMode, "Spawning player in non gameplay game mode", 5.f, FColor::Red);
     
    // Set initial state
    SetState(PlayerStateEnum::WANDER_IDLE, true);
}

void AInspectPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState)
        CurrentState->Tick(DeltaTime);

    Move();
    Look(DeltaTime);

    SearchInteractables();
}

void AInspectPlayer::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind Camera
    InputComponent->BindAxis("LookUp", this, &AInspectPlayer::AxisPitch);
    InputComponent->BindAxis("Turn", this, &AInspectPlayer::AxisYaw);

    // Bind Movement Axes
    InputComponent->BindAxis("Forward", this, &AInspectPlayer::AxisForwards);
    InputComponent->BindAxis("Right", this, &AInspectPlayer::AxisRight);

    // Bind Movement Actions
    InputComponent->BindAction("Run", IE_Pressed, this, &AInspectPlayer::ActionRunPressed).bConsumeInput = false;
    InputComponent->BindAction("Run", IE_Released, this, &AInspectPlayer::ActionRunReleased).bConsumeInput = false;

    InputComponent->BindAction("Sneak", IE_Pressed, this, &AInspectPlayer::ActionSneakPressed).bConsumeInput = false;
    InputComponent->BindAction("Sneak", IE_Released, this, &AInspectPlayer::ActionSneakReleased).bConsumeInput = false;

    InputComponent->BindAction("Interact", IE_Pressed, this, &AInspectPlayer::ActionInteractPressed).bConsumeInput = false;
    InputComponent->BindAction("Interact", IE_Released, this, &AInspectPlayer::ActionInteractReleased).bConsumeInput = false;

    InputComponent->BindAction("AnyKey", IE_Pressed, this, &AInspectPlayer::OnAnyKey).bConsumeInput = false;
}

// Components

FPlayerResponse AInspectPlayer::GetDoorsPlayer()
{
    return FPlayerResponse(Player, (Player != nullptr));
}

UCameraComponent *AInspectPlayer::GetCameraCmp()
{
    return CameraCmp;
}

USpringArmComponent *AInspectPlayer::GetSpringArmCmp()
{
    return SpringArmCmp;
}

AInspectPlayerController *AInspectPlayer::GetPlayerController()
{
    return PlayerController;
}

APlayerCameraManager *AInspectPlayer::GetCameraManager()
{
    return PlayerCameraManager;
}

// States

PlayerStateEnum AInspectPlayer::GetState()
{
    return CurrentStateEnum;
}

void AInspectPlayer::SetState(PlayerStateEnum NewState, bool IsSuperState)
{
    CurrentStateEnum = NewState;

    OnStateChanged(NewState);

    Logger::LogEnum(ShowDebugStateChanges, NewState, 1000.0f, FColor::Emerald, 0);

    if (!IsSuperState)
        return;

    if (CurrentState)
    {
        wasSneaking = (CurrentState == (UPlayerStateBase *)StateSneaking) && !wasSneaking;

        CurrentState->Disable();
    }

    switch (NewState)
    {
    case PlayerStateEnum::DEFAULT:
    case PlayerStateEnum::WANDER_IDLE:
    case PlayerStateEnum::WANDER_WALK:
    case PlayerStateEnum::WANDER_RUN:
        CurrentState = (UPlayerStateBase *)(StateWandering);
        break;
    case PlayerStateEnum::SNEAK_IDLE:
    case PlayerStateEnum::SNEAK_WALK:
        CurrentState = (UPlayerStateBase *)(StateSneaking);
        break;

    case PlayerStateEnum::NONE:
        CurrentState = (UPlayerStateBase *)(nullptr);
        break;
    default:
        break;
    }

    if (CurrentState)
        CurrentState->Enable();
}

// Interactable Detection

void AInspectPlayer::SetTraceChannel(ETraceTypeQuery Channel)
{
    CurrentTraceChannel = Channel;
}

void AInspectPlayer::ResetTraceChannel()
{
    CurrentTraceChannel = ETraceTypeQuery::TraceTypeQuery1;
}

void AInspectPlayer::SearchInteractables()
{

    auto *World = GetWorld();
    auto *Cam = GetCameraCmp();
    auto Collided = false;

    auto Start = FVector::ZeroVector;
    auto End = FVector::ZeroVector;

    auto Result = FHitResult();

    const auto RayCast = [&](float Length, float Radius, FLinearColor Color) {
        Start = Cam->GetComponentLocation() + Cam->GetForwardVector() * Radius;
        End = Cam->GetComponentLocation() + Cam->GetForwardVector() * (Length - Radius);

        return ShowDebugInteractables
                   ? UKismetSystemLibrary::SphereTraceSingle(
                         World, Start, End, Radius, ETraceTypeQuery::TraceTypeQuery1, false, {Player},
                         EDrawDebugTrace::ForOneFrame, Result, true, Color, Color)
                   : UKismetSystemLibrary::SphereTraceSingle(
                         World, Start, End, Radius, ETraceTypeQuery::TraceTypeQuery1, false, {Player},
                         EDrawDebugTrace::None, Result, true);
    };

    // Try to get interactable through raycast
    if (RayCast(SmallRayLength, SmallRayRadius, FLinearColor::Yellow))
        DetectedInteractable = Cast<UObject>(Result.GetActor());
    else if (RayCast(BigRayLength, BigRayRadius, FLinearColor::Red))
        DetectedInteractable = Cast<UObject>(Result.GetActor());
    else
        DetectedInteractable = nullptr;

    
    if (DetectedInteractable)
        Logger::Log(ShowDebugInteractables, DetectedInteractable->GetName(), 1000.f, FColor::Emerald, 2);
    else
        Logger::Log(ShowDebugInteractables, "No Interactable", 1000.f, FColor::Emerald, 2);

    // What happened
    if (OldDetectedInteractable == nullptr && DetectedInteractable != nullptr)
        InteractableFound(DetectedInteractable);
    if (OldDetectedInteractable != nullptr && DetectedInteractable == nullptr)
        InteractableLost(OldDetectedInteractable);
    if (DetectedInteractable != OldDetectedInteractable)
        InteractableChanged(DetectedInteractable, OldDetectedInteractable);

    OldDetectedInteractable = DetectedInteractable;
}

void AInspectPlayer::InteractableFound(UObject *New)
{
    SetOutline(New, true);
    OnInteractableFound();
}

void AInspectPlayer::InteractableChanged(UObject *New, UObject *Old)
{
    SetOutline(Old, false);
    SetOutline(New, true);
}

void AInspectPlayer::InteractableLost(UObject *Old)
{
    SetOutline(Old, false);
    OnInteractableLost();
}

void AInspectPlayer::SetOutline(UObject *Obj, bool Value)
{
    if (!Obj)
        return;

    auto *Actor = Cast<AActor>(Obj);

    if (!Actor)
        return;

    if (!IsImplemented(Actor, UOutlinableInterface::StaticClass()))
        return;

    auto Active = IOutlinableInterface::Execute_IsOutlinable(Actor);

    if (!Active)
        return;

    if (!Value && IOutlinableInterface::Execute_KeepOutline(Actor))
        return;

    auto Meshes = IOutlinableInterface::Execute_GetMeshes(Actor);

    if (Meshes.Num() == 0)
        Actor->GetComponents<UStaticMeshComponent>(Meshes);

    for (auto *Comp : Meshes)
        if (Comp)
            Comp->SetRenderCustomDepth(Value);
}

// Input

FVector2D AInspectPlayer::GetMovementAxis()
{
    return MoveAxis;
}

void AInspectPlayer::SetMovementAxis(FVector2D Value)
{
    MoveAxis = Value;
}

FVector2D AInspectPlayer::GetLookAxis()
{
    return LookAxis;
}

void AInspectPlayer::SetLookAxis(FVector2D Value)
{
    LookAxis = Value;
}

void AInspectPlayer::AxisPitch(float Amount)
{
    LookAxis.Y = Amount;
}

void AInspectPlayer::AxisYaw(float Amount)
{
    LookAxis.X = Amount;
}

void AInspectPlayer::AxisForwards(float Amount)
{
    MoveAxis.Y = Amount;
}

void AInspectPlayer::AxisRight(float Amount)
{
    MoveAxis.X = Amount;
}

void AInspectPlayer::ActionRunPressed()
{
    bActionRunState = true;

    if (CurrentState)
        CurrentState->ActionRunPressed();
}

void AInspectPlayer::ActionRunReleased()
{
    bActionRunState = false;

    if (CurrentState)
        CurrentState->ActionRunReleased();
}

void AInspectPlayer::ActionSneakPressed()
{
    bActionSneakState = true;

    if (CurrentState)
        CurrentState->ActionSneakPressed();
}

void AInspectPlayer::ActionSneakReleased()
{
    bActionSneakState = false;

    if (CurrentState)
        CurrentState->ActionSneakReleased();
}

void AInspectPlayer::ActionInteractPressed()
{
    bActionInteractState = true;
}

void AInspectPlayer::ActionInteractReleased()
{

    bActionInteractState = false;

    if (!DetectedInteractable)
        return;
    
    if (IsImplemented(DetectedInteractable, UInteractableInterface::StaticClass()))
    {
        OnTap();
        IInteractableInterface::Execute_OnTap(DetectedInteractable);
        Logger::Log(ShowDebugTap, "OnTap", 1000.f, FColor::Emerald, 4);
    }
}

void AInspectPlayer::Move()
{
    if (MoveAxis.Size() > 1.f)
    {
        MoveAxis = MoveAxis.GetSafeNormal();
    }

    // Regular Movement
    auto Fwd = CameraCmp->GetForwardVector().GetSafeNormal2D() * MoveAxis.Y;
    auto Right = CameraCmp->GetRightVector().GetSafeNormal2D() * MoveAxis.X;
    auto Dir = Fwd + Right;
    auto Norm = Dir.Size() > 1.f ? Dir.GetSafeNormal2D() : Dir;
    Player->AddMovementInput(Norm, Dir.Size());
    
}

void AInspectPlayer::Look(float DeltaSeconds)
{
    AddControllerPitchInput(fPitchSensibility * LookAxis.Y * DeltaSeconds);
    AddControllerYawInput(fYawSensibility * LookAxis.X * DeltaSeconds);
}

void AInspectPlayer::OnAnyKey(FKey Key)
{
    if (!GameMode)
        return;

    GameMode->OnAnyKey(Key);
}
