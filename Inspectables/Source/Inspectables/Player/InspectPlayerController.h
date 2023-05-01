#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InspectPlayerController.generated.h"

UCLASS()
class AInspectPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
		AInspectPlayerController();

		virtual void BeginPlay() override;
};
