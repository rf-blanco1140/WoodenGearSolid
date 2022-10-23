// Fill out your copyright notice in the Description page of Project Settings.


#include "KidCharacter.h"
#include "Camera/CameraComponent.h"
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

	PlayerInputComponent->BindAxis("MoveForward", this, &AKidCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AKidCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AKidCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AKidCharacter::LookUp);

	PlayerInputComponent->BindAxis("TurnRate", this, &AKidCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AKidCharacter::LookUpAtRate);
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
	//if (!childController->CanMove())
		//return;
	AddControllerPitchInput(rate * BaseLookRate * GetWorld()->GetDeltaSeconds());
}
