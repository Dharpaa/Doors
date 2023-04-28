#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DoorsEnemy.generated.h"

UCLASS()
class ADoorsEnemy : public ACharacter
{
    GENERATED_BODY()


public:
	UPROPERTY(Category = "Doors_AIPerception", EditAnywhere, BlueprintReadWrite)
		float fHeightDistanceHear = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ADoorsTargetPoint*> WayPoints;

	UPROPERTY(/*EditAnywhere, BlueprintReadOnly*/)
	bool PatrolInOrder = true;
};
