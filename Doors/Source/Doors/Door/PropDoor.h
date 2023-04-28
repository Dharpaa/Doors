#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "PropDoor.generated.h"

class ADoorsEnemy;
class ADoorsPlayer;
class UCurveFloat;
class UItemAsset;

UCLASS()
class APropDoor : public AActor
{
    GENERATED_BODY()

  public:
    // Sets default values for this component's properties
    APropDoor();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Components
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent *RootCmp; 

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent *CentrePointCmp;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent *StaticMeshCmp;
	

    // Public Methods
    UFUNCTION(BlueprintCallable, Category = "DoorMethods")
    void Interact(float OpenDuration = 1.0f, float CloseDuration = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "DoorMethods")
	void InteractByAI(ADoorsEnemy* OtherActor, float OpenDuration = 1.0f, float CloseDuration = 1.0f);
	
    UFUNCTION(BlueprintCallable, Category = "DoorMethods")
    void OpenDoor(float Duration, UCurveFloat* OpenCurve = nullptr,UCurveFloat* CloseCurve = nullptr, ADoorsEnemy* OtherActor = nullptr,  bool bIsPlayer = false);

    UFUNCTION(BlueprintCallable, Category = "DoorMethods")
    void CloseDoor(float Duration, UCurveFloat* OpenCurve = nullptr,UCurveFloat* CloseCurve = nullptr);

    UFUNCTION(BlueprintPure, Category = "DoorMethods")
    bool IsOpened();

  private:
    // Called when the game starts
    virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "DoorSettings")
	bool NeverOpening = false;

    UPROPERTY()
    bool bHasMoved = false;
		
    // Door params
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "DoorSettings")
	bool IsOpenAtStart = false;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "DoorSettings")
    float OpenCloseAngle;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), BlueprintReadWrite, Category = "DoorSettings")
	float PlayerOpenDuration;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), BlueprintReadWrite, Category = "DoorSettings")
	float PlayerCloseDuration;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), BlueprintReadWrite, Category = "DoorSettings")
	float EnemyOpenDuration;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), BlueprintReadWrite, Category = "DoorSettings")
	float EnemyCloseDuration;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), BlueprintReadWrite, Category = "DoorCurves")
	UCurveFloat* EnemyOpenCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), BlueprintReadWrite, Category = "DoorCurves")
	UCurveFloat* EnemyCloseCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), BlueprintReadWrite, Category = "DoorCurves")
	UCurveFloat* PlayerOpenCurve;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), BlueprintReadWrite, Category = "DoorCurves")
	UCurveFloat* PlayerCloseCurve;
	
    //Private Methods
    UFUNCTION()
    void ToggleDoor(float OpenDuration, float CloseDuration, UCurveFloat* OpenCurve = nullptr, UCurveFloat* CloseCurve = nullptr);

    void WhileMoving(float DeltaTime); 
	
    UPROPERTY()
    bool IsOpen = false;
    UPROPERTY()
    bool IsMoving = false;

    UPROPERTY(SaveGame)
    FRotator EndRotation = FRotator::ZeroRotator;
    UPROPERTY(SaveGame)
    FRotator StartRotation = FRotator::ZeroRotator;
    
    float MoveTimer = 0.0f;
    float MoveDuration = 0.0f;
    UCurveFloat* MoveOpenCurve = nullptr;
    UCurveFloat* MoveCloseCurve = nullptr;
};