#pragma once

#include "InteractSceneComponentBase.h"
#include "Components/PointLightComponent.h"
#include "InteractInspectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FInspectCancelSignature, UInteractInspectComponent, OnInspectCancel);

UCLASS(meta = (BlueprintSpawnableComponent))
class UInteractInspectComponent: public UInteractSceneComponentBase
{
	GENERATED_BODY()
	
public:
	UInteractInspectComponent();
	virtual void BeginPlay() override;
	virtual void OnComponentCreated() override;
	UFUNCTION(Category = "Inspect", BlueprintCallable)
	void Inspect();

	UFUNCTION(Category = "Inspect", BlueprintCallable)
	void CancelInspect();

	virtual void TickComponent(float DeltaTime,ELevelTick TickType,
	FActorComponentTickFunction *ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspect")
	float DistanceFromCamera = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspect")
	UStaticMeshComponent* InspectMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspect")
	UPointLightComponent* PointLightComponent;

	UPROPERTY(BlueprintAssignable)
	FInspectCancelSignature OnInspectCancel;
	
	bool InspectCanceled = false;
private:
	FVector2D PrevLookAxis = FVector2D::ZeroVector;
	FVector2D DeltaLookAxis = FVector2D::ZeroVector;
	FTransform OldInspectableTransform = FTransform::Identity;
	
	UPROPERTY(EditAnywhere, Category = "Inspect")
	float RotationSpeed = 1.0f;
};

