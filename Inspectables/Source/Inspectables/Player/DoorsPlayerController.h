#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "DoorsPlayerController.generated.h"

UCLASS()
class ADoorsPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
		ADoorsPlayerController();

		virtual void BeginPlay() override;

	private:
		UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAIPerceptionStimuliSourceComponent *MskPerceptionStimuliSourceComponent;
};
