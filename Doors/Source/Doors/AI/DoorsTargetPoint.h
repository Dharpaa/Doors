#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "DoorsTargetPoint.generated.h"

/**
 *
 */
UCLASS()
class ADoorsTargetPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = Waypoint, EditAnywhere, BlueprintReadOnly)
		int32 Position;

	UPROPERTY(Category = Waypoint, EditAnywhere, BlueprintReadOnly)
		float WaitTime;

	float GetWaitTime();
};
