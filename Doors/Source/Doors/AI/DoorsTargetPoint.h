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
	UPROPERTY(Category = MskWaypoint, EditAnywhere, BlueprintReadOnly)
		int32 Position;

	UPROPERTY(Category = MskWaypoint, EditAnywhere, BlueprintReadOnly)
		float WaitTime;

	float GetWaitTime();
};
