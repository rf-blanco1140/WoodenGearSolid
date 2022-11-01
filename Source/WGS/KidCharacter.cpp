// Fill out your copyright notice in the Description page of Project Settings.


#include "KidCharacter.h"
#include "KidController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AKidCharacter::AKidCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 0.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = false; // Rotate arm based on controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = true;

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
	CurrentState = EKidState::Walking;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AKidCharacter::CheckRoof);

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = MinPitch;
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = MaxPitch;
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

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &AKidCharacter::Turn);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &AKidCharacter::LookUp);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AKidCharacter::ToggleCrouching);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AKidCharacter::StartRunning);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AKidCharacter::EndRunning);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AKidCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	KidController = Cast<AKidController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (GetKidController())
	{
		PlayerInputComponent->BindAction("Grab", IE_Pressed, KidController, &AKidController::InteractWithSelected);
	}
}

void AKidCharacter::Jump()
{
	if (!GetKidController()->CanMove() || CurrentState == EKidState::Climbing)
		return;

	Super::Jump();
}

void AKidCharacter::MoveForward(float value)
{
	if (!GetKidController()->CanMove())
		return;
	if (value == 0)
		return;

	if (CurrentState == EKidState::Climbing)
	{
		FHitResult HitResult;
		SetActorLocation(GetActorLocation() + FVector::ZAxisVector * value * ClimbingSpeed, true, &HitResult);
	}
	else
	{
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void AKidCharacter::MoveRight(float value)
{
	if (!GetKidController()->CanMove() || CurrentState == EKidState::Climbing)
		return;
	
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, rotation.Yaw, 0.f);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}

void AKidCharacter::Turn(float value)
{
	if (!GetKidController()->CanMove())
		return;
	AddControllerYawInput(value);
}

void AKidCharacter::LookUp(float value)
{
	if (!GetKidController()->CanMove())
		return;
	AddControllerPitchInput(value);
}

void AKidCharacter::ToggleCrouching()
{
	if (CurrentState == EKidState::Crouching)
	{
		CurrentState = EKidState::Walking;
		GetCapsuleComponent()->SetCapsuleHalfHeight(RegularHeight / 2, false);
		GetMesh()->SetRelativeLocation(FVector(0,0, -RegularHeight / 2), false);
	}
	else
	{
		CurrentState = EKidState::Crouching;
		GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchingHeight / 2, true);
		GetMesh()->SetRelativeLocation(FVector(0,0, -CrouchingHeight / 2), false);
	}
}

void AKidCharacter::StartClimbing()
{
	if (CurrentState == EKidState::Crouching)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(RegularHeight / 2, false);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -RegularHeight / 2), false);
	}
	
	GetCharacterMovement()->Deactivate();
	CurrentState = EKidState::Climbing;

}

void AKidCharacter::StopClimbing()
{
	GetCharacterMovement()->Activate();
	CurrentState = EKidState::Walking;
}

EKidState AKidCharacter::GetCurrentState()
{
	return CurrentState;
}


void AKidCharacter::CheckRoof(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit normal: %s"), *Hit.Normal.ToString());
	if (Hit.Normal == FVector(0,0,-1))
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchingHeight / 2, true);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -CrouchingHeight / 2), false);
		CurrentState = EKidState::Crouching;
	}
	else if (Hit.Normal == FVector(0, 0, 1) && CurrentState == EKidState::Climbing)
	{
		StopClimbing();
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

AKidController* AKidCharacter::GetKidController() const
{
	return KidController;
}

