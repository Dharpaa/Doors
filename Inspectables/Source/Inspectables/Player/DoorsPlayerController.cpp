#include "DoorsPlayerController.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "../GameMode/DoorsGameplayGameMode.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

ADoorsPlayerController::ADoorsPlayerController()
{
    MskPerceptionStimuliSourceComponent =
        CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliComponent"));

    // Register Source Senses
    MskPerceptionStimuliSourceComponent->bAutoRegister = true;
    MskPerceptionStimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
    MskPerceptionStimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Hearing>());
}

void ADoorsPlayerController::BeginPlay()
{
    Super::BeginPlay();

    auto* GM = Cast<ADoorsGameplayGameMode>(UGameplayStatics::GetGameMode(this));

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
