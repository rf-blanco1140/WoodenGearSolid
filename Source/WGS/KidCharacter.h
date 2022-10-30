#pragma once

#include <CoreMinimal.h>
#include "GameFramework/Character.h"
#include "KidCharacter.generated.h"

UENUM(BlueprintType)
enum class EKidState : uint8
{
	Walking,
	Crouching,
	Climbing,
};

UCLASS()
class WGS_API AKidCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKidCharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BaseLookRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float MinPitch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float MaxPitch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kid")
	float RegularHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kid")
	float CrouchingHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kid")
	float NormalSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kid")
	float SprintSpeed;

protected:
	UPROPERTY(BlueprintReadOnly)
	EKidState CurrentState;
	UPROPERTY()
	class AKidController* KidController;

	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float value);
	void MoveRight(float value);
	void LookUp(float value);
	void Turn(float value);
	void TurnAtRate(float rate);
	void LookUpAtRate(float rate);
	void ToggleCrouching();
	void StartRunning();
	void EndRunning();
	void Jump();
	
	UFUNCTION()
	void CheckRoof(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	void StartClimbing();
	void StopClimbing();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EKidState GetCurrentState();
	class AKidController* GetKidController() const;
};