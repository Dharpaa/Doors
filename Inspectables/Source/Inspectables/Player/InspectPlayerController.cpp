#include "InspectPlayerController.h"
#include "../GameMode/InspectGameplayGameMode.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

AInspectPlayerController::AInspectPlayerController()
{
}

void AInspectPlayerController::BeginPlay()
{
    Super::BeginPlay();

    auto* GM = Cast<AInspectGameplayGameMode>(UGameplayStatics::GetGameMode(this));

    if (!GM)
        return;

    auto* PlayerStart = (AActor*)nullptr;

    if (!PlayerStart)
        PlayerStart = GM->FindPlayerStart_Implementation(this, "Start");
    
    if (PlayerStart)
    {
        auto Loc = PlayerStart->GetActorTransform().GetLocation();
        auto Rot = PlayerStart->GetActorRotation();

        this->ClientSetLocation(Loc, Rot);
    }
}
