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
	UStaticMeshComponent* FieldOfView;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ActionableRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PersonalSpace;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Values")
	float AlertTime;

	UFUNCTION()
	void EnteredFieldOfView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ExitedFieldOfView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void EnteredCatchingRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ToggleStateVisuals();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAlertState(float Percentage);

protected:
	UPROPERTY(BlueprintReadOnly)
	EEnemyState CurrentState;
	UPROPERTY(BlueprintReadOnly)
	float CurrentAlertDelay;
	UPROPERTY()
	class AKidController* KidController;
	
	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;
};

UCLASS()
class WGS_API ARotatingEnemy : public AEnemy
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Values")
	float RotateTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Values")
	TArray<FRotator> Rotations;

protected:
	
	UPROPERTY(BlueprintReadOnly)
	float CurrentRotateDelay;
	UPROPERTY(BlueprintReadOnly)
	int RotationIndex;
	
	void Tick(float DeltaTime) override;
};

UCLASS()
class WGS_API UAlertIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateAlertIndicator(const float Percentage);
};