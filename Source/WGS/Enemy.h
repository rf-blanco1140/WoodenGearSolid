#pragma once

#include <CoreMinimal.h>
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	ActiveAlert,
	PassiveAlert,
};

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
	class UStaticMeshComponent* ActionableRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* PersonalSpace;
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
	void UpdateAlertIndicator();

protected:
	UPROPERTY(BlueprintReadOnly)
	EEnemyState CurrentState;
	UPROPERTY(BlueprintReadOnly)
	float CurrentAlertDelay;
	UPROPERTY()
	class AKidController* KidController;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Tick(float DeltaTime) override;
};