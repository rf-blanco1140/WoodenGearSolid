// Fill out your copyright notice in the Description page of Project Settings.


#include "KidCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"

AKidCharacter::AKidCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 0.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	BaseTurnRate = 65.f;
	BaseLookRate = 65.f;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // Rotation rate, to rotate only in yaw
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AKidCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->SetCapsuleHalfHeight(RegularHeight / 2, true);
	bIsCrouching = false;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AKidCharacter::CheckRoof);
}

// Called every frame
void AKidCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AKidCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AKidCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AKidCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AKidCharacter::Turn);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AKidCharacter::LookUp);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &AKidCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &AKidCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AKidCharacter::ToggleCrouching);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AKidCharacter::StartRunning);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AKidCharacter::EndRunning);
}

void AKidCharacter::MoveForward(float value)
{
	//if (!childController->CanMove())
		//return;
	// Find out which way is forward
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, rotation.Yaw, 0.f);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, value);
}

void AKidCharacter::MoveRight(float value)
{
	//if (!childController->CanMove())
		//return;
	// Find out which way is forward
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, rotation.Yaw, 0.f);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}

void AKidCharacter::Turn(float value)
{
	//if (!childController->CanMove())
		//return;
	AddControllerPitchInput(-value);
}

void AKidCharacter::LookUp(float value)
{
	//if (!childController->CanMove())
		//return;
	AddControllerYawInput(value);
}

void AKidCharacter::TurnAtRate(float rate)
{
	//if (!childController->CanMove())
		//return;
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AKidCharacter::LookUpAtRate(float rate)
{
	return;
	//if (!childController->CanMove())
		//return;
	AddControllerPitchInput(rate * BaseLookRate * GetWorld()->GetDeltaSeconds());
}

void AKidCharacter::ToggleCrouching()
{
	if (bIsCrouching)
	{
		bIsCrouching = false;
		GetCapsuleComponent()->SetCapsuleHalfHeight(RegularHeight / 2, false);
	}
	else
	{
		bIsCrouching = true;
		GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchingHeight / 2, true);
	}
}

void AKidCharacter::CheckRoof(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit normal: %s"), *Hit.Normal.ToString());
	if (Hit.Normal == FVector(0,0,-1))
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchingHeight / 2, true);
		bIsCrouching = true;
	}
}

void AKidCharacter::StartRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}
void AKidCharacter::EndRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}
