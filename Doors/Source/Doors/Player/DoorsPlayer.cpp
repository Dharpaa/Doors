#include "DoorsPlayer.h"
#include "DoorsPlayerController.h"
#include "States/PlayerStateBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "../GameMode/DoorsGameplayGameMode.h"
#include "../Interfaces/InteractableInterface.h"
#include "../Interfaces/OutlinableInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../Logger.h"

#define IsImplemented UKismetSystemLibrary::DoesImplementInterface

// Base

ADoorsPlayer *ADoorsPlayer::DoorsPlayer = nullptr;

ADoorsPlayer::ADoorsPlayer()
{
    DoorsPlayer = this;

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

void ADoorsPlayer::BeginPlay()
{
    Super::BeginPlay();

    auto *World = GetWorld();

    // Log reserve space
    Logger::Log(ShowDebugStateChanges, "-", 5.f, FColor::Transparent, 0);
    Logger::Log(ShowDebugInteractables, "-", 5.f, FColor::Transparent, 1);
    Logger::Log(ShowDebugTap, "-", 5.f, FColor::Transparent, 2);
    Logger::Log(ShowDebugTap, "-", 5.f, FColor::Transparent, 3);

    // Cache PlayerController
    PlayerController = Cast<ADoorsPlayerController>(GetController());
    if (PlayerController)
        PlayerCameraManager = PlayerController->PlayerCameraManager;

    // Cache GameMode
    GameMode = Cast<ADoorsGameplayGameMode>(UGameplayStatics::GetGameMode(World));
    Logger::Log(!GameMode, "Spawning player in non gameplay game mode", 5.f, FColor::Red);
     
    // Set initial state
    SetState(PlayerStateEnum::WANDER_IDLE, true);
}

void ADoorsPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState)
        CurrentState->Tick(DeltaTime);

    Move();
    Look(DeltaTime);

    SearchInteractables();
}

void ADoorsPlayer::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind Camera
    InputComponent->BindAxis("LookUp", this, &ADoorsPlayer::AxisPitch);
    InputComponent->BindAxis("Turn", this, &ADoorsPlayer::AxisYaw);

    // Bind Movement Axes
    InputComponent->BindAxis("Forward", this, &ADoorsPlayer::AxisForwards);
    InputComponent->BindAxis("Right", this, &ADoorsPlayer::AxisRight);

    // Bind Movement Actions
    InputComponent->BindAction("Run", IE_Pressed, this, &ADoorsPlayer::ActionRunPressed).bConsumeInput = false;
    InputComponent->BindAction("Run", IE_Released, this, &ADoorsPlayer::ActionRunReleased).bConsumeInput = false;

    InputComponent->BindAction("Sneak", IE_Pressed, this, &ADoorsPlayer::ActionSneakPressed).bConsumeInput = false;
    InputComponent->BindAction("Sneak", IE_Released, this, &ADoorsPlayer::ActionSneakReleased).bConsumeInput = false;

    InputComponent->BindAction("Interact", IE_Pressed, this, &ADoorsPlayer::ActionInteractPressed).bConsumeInput = false;
    InputComponent->BindAction("Interact", IE_Released, this, &ADoorsPlayer::ActionInteractReleased).bConsumeInput = false;

    InputComponent->BindAction("AnyKey", IE_Pressed, this, &ADoorsPlayer::OnAnyKey).bConsumeInput = false;
}

// Components

FDoorsPlayerResponse ADoorsPlayer::GetDoorsPlayer()
{
    return FDoorsPlayerResponse(DoorsPlayer, (DoorsPlayer != nullptr));
}

UCameraComponent *ADoorsPlayer::GetCameraCmp()
{
    return CameraCmp;
}

USpringArmComponent *ADoorsPlayer::GetSpringArmCmp()
{
    return SpringArmCmp;
}

ADoorsPlayerController *ADoorsPlayer::GetPlayerController()
{
    return PlayerController;
}

APlayerCameraManager *ADoorsPlayer::GetCameraManager()
{
    return PlayerCameraManager;
}

// States

PlayerStateEnum ADoorsPlayer::GetState()
{
    return CurrentStateEnum;
}

void ADoorsPlayer::SetState(PlayerStateEnum NewState, bool IsSuperState)
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

void ADoorsPlayer::SetTraceChannel(ETraceTypeQuery Channel)
{
    CurrentTraceChannel = Channel;
}

void ADoorsPlayer::ResetTraceChannel()
{
    CurrentTraceChannel = ETraceTypeQuery::TraceTypeQuery1;
}

void ADoorsPlayer::SearchInteractables()
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
                         World, Start, End, Radius, ETraceTypeQuery::TraceTypeQuery1, false, {DoorsPlayer},
                         EDrawDebugTrace::ForOneFrame, Result, true, Color, Color)
                   : UKismetSystemLibrary::SphereTraceSingle(
                         World, Start, End, Radius, ETraceTypeQuery::TraceTypeQuery1, false, {DoorsPlayer},
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

void ADoorsPlayer::InteractableFound(UObject *New)
{
    SetOutline(New, true);
    OnInteractableFound();
}

void ADoorsPlayer::InteractableChanged(UObject *New, UObject *Old)
{
    SetOutline(Old, false);
    SetOutline(New, true);
}

void ADoorsPlayer::InteractableLost(UObject *Old)
{
    SetOutline(Old, false);
    OnInteractableLost();
}

void ADoorsPlayer::SetOutline(UObject *Obj, bool Value)
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

FVector2D ADoorsPlayer::GetMovementAxis()
{
    return MoveAxis;
}

void ADoorsPlayer::SetMovementAxis(FVector2D Value)
{
    MoveAxis = Value;
}

FVector2D ADoorsPlayer::GetLookAxis()
{
    return LookAxis;
}

void ADoorsPlayer::SetLookAxis(FVector2D Value)
{
    LookAxis = Value;
}

void ADoorsPlayer::AxisPitch(float Amount)
{
    LookAxis.Y = Amount;
}

void ADoorsPlayer::AxisYaw(float Amount)
{
    LookAxis.X = Amount;
}

void ADoorsPlayer::AxisForwards(float Amount)
{
    MoveAxis.Y = Amount;
}

void ADoorsPlayer::AxisRight(float Amount)
{
    MoveAxis.X = Amount;
}

void ADoorsPlayer::ActionRunPressed()
{
    bActionRunState = true;

    if (CurrentState)
        CurrentState->ActionRunPressed();
}

void ADoorsPlayer::ActionRunReleased()
{
    bActionRunState = false;

    if (CurrentState)
        CurrentState->ActionRunReleased();
}

void ADoorsPlayer::ActionSneakPressed()
{
    bActionSneakState = true;

    if (CurrentState)
        CurrentState->ActionSneakPressed();
}

void ADoorsPlayer::ActionSneakReleased()
{
    bActionSneakState = false;

    if (CurrentState)
        CurrentState->ActionSneakReleased();
}

void ADoorsPlayer::ActionInteractPressed()
{
    bActionInteractState = true;
}

void ADoorsPlayer::ActionInteractReleased()
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

void ADoorsPlayer::Move()
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
    DoorsPlayer->AddMovementInput(Norm, Dir.Size());
    
}

void ADoorsPlayer::Look(float DeltaSeconds)
{
    AddControllerPitchInput(fPitchSensibility * LookAxis.Y * DeltaSeconds);
    AddControllerYawInput(fYawSensibility * LookAxis.X * DeltaSeconds);
}

void ADoorsPlayer::OnAnyKey(FKey Key)
{
    if (!GameMode)
        return;

    GameMode->OnAnyKey(Key);
}
