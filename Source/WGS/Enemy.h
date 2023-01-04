#pragma once

#include <CoreMinimal.h>
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	ActiveAlert,
	PassiveAlert,
};

class UStaticMeshComponent;

UCLASS()
class WGS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* FieldOfView;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDecalComponent* FOVShadow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UPaperSpriteComponent* FaceSprite;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Values")
	float AlertTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Values")
	float MaxFOVLength = 1000;
	float FOVHeight = 10;
	float FOVAngle = 30;
	float FOVScaleFactor = 0.01;

	UFUNCTION()
	void EnteredFieldOfView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ExitedFieldOfView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleStateVisuals();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateIndicatorState(float Percentage);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Values")
	bool bShouldSit;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Values")
	bool bShouldWalk;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation Values")
	bool bShouldSleep;
	UPROPERTY(BlueprintReadOnly)
	EEnemyState CurrentState;
	UPROPERTY(BlueprintReadOnly)
	float CurrentAlertDelay;
	UPROPERTY()
	class AKidController* KidController;
	UPROPERTY()
	class AKidCharacter* Kid;
	
	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void CheckFOVLength();
	float CheckInDirection(FVector);
	void KillChild();
	bool bIsSeeingChild;
};

UCLASS()
class WGS_API AIntermitentEnemy : public AEnemy
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Values")
	float ActiveTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Values")
	float InactiveTime;

protected:

	UPROPERTY(BlueprintReadOnly)
	float CurrentActiveDelay;
	UPROPERTY(BlueprintReadOnly)
	float CurrentInactiveDelay;

	FVector FOVScale;

	void Tick(float DeltaTime) override;
}; 

UCLASS()
class WGS_API UEnemyIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateIndicator(const float Percentage);
};

UCLASS()
class WGS_API ARotatingEnemy : public AEnemy
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Values")
	float RotateTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Values")
	TArray<FRotator> Rotations;

protected:
	
	UPROPERTY(BlueprintReadOnly)
	float CurrentRotateDelay;
	UPROPERTY(BlueprintReadOnly)
	int RotationIndex;
	
	void Tick(float DeltaTime) override;
};

UCLASS()
class WGS_API APatrolingEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	APatrolingEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Values")
	class USplineComponent* PatrolRoad;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Values")
	TArray<float> DelayPerStop;

	FVector GetWorldLocationByIndex(int Index) const;
	int GetLastIndex() const;

protected:

	UPROPERTY(BlueprintReadOnly)
	float InPositionDelay;
	UPROPERTY(BlueprintReadOnly)
	int PatrolIndex;
	UPROPERTY(BlueprintReadWrite)
	bool bIsWalking;
	UPROPERTY(BlueprintReadWrite)
	bool bReachedDestination;

	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;
	float GetCurrentDelay();

	UFUNCTION(BlueprintImplementableEvent)
	void MoveToLocation(FVector Location);
	UFUNCTION(BlueprintImplementableEvent)
	void StopMoving();
};