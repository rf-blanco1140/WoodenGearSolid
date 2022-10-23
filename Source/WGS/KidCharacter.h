#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KidCharacter.generated.h"

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kid")
		float RegularHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kid")
		float CrouchingHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kid")
		float NormalSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kid")
		float SprintSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool bIsCrouching;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	class AKidController* GetKidController();

	void MoveForward(float value);
	void MoveRight(float value);
	void LookUp(float value);
	void Turn(float value);
	void TurnAtRate(float rate);
	void LookUpAtRate(float rate);
	void ToggleCrouching();
	UFUNCTION()
	void CheckRoof(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void StartRunning();
	void EndRunning();
};