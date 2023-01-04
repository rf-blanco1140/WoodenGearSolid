#include "Enemy.h"
#include "KidCharacter.h"
#include "KidController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Components/DecalComponent.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	FieldOfView = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Field Of View"));
	FieldOfView->SetCollisionResponseToAllChannels(ECR_Ignore);
	FieldOfView->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FieldOfView->SetupAttachment(RootComponent);

	FOVShadow = CreateDefaultSubobject<UDecalComponent>(TEXT("Shadow"));
	FOVShadow->SetupAttachment(RootComponent);

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCharacterMovement()->GravityScale = 0;

	FaceSprite = CreateDefaultSubobject<UPaperSpriteComponent>("Face");
	FaceSprite->SetupAttachment(GetMesh(), TEXT("FaceSocket"));
	FaceSprite->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EEnemyState::Idle;
	FieldOfView->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::EnteredFieldOfView);
	FieldOfView->OnComponentEndOverlap.AddDynamic(this, &AEnemy::ExitedFieldOfView);
	Kid = Cast<AKidCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	UpdateIndicatorState(0);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (KidController)
	{
		if (!KidController->CanMove())
		{
			return;
		}
	}

	CheckFOVLength();

	if (!KidController)
		return;

	if (bIsSeeingChild)
	{
		switch (KidController->GetStealthState())
		{
			case EStealthState::Exposed:
				KillChild();
				return;
				break;
			case EStealthState::PartiallyHidden:
				if (CurrentState == EEnemyState::Idle)
				{
					CurrentState = EEnemyState::ActiveAlert;
					ToggleStateVisuals();
				}
				break;
			case EStealthState::TotallyHidden:
				if (CurrentState != EEnemyState::Idle)
				{
					CurrentState = EEnemyState::PassiveAlert;
					ToggleStateVisuals();
				}
				break;
		}
	}
	
	if (CurrentState == EEnemyState::ActiveAlert)
	{
		CurrentAlertDelay += DeltaTime;
		KidController->UpdateAlertState(CurrentAlertDelay / AlertTime);
		if (CurrentAlertDelay >= AlertTime)
		{
			KillChild();
		}
	}
	else if (CurrentState == EEnemyState::PassiveAlert)
	{
		CurrentAlertDelay -= DeltaTime;
		KidController->UpdateAlertState(CurrentAlertDelay / AlertTime);
		if (CurrentAlertDelay <= 0)
		{
			CurrentState = EEnemyState::Idle;
			ToggleStateVisuals();
		}
	}
}

void AEnemy::KillChild()
{
	bIsSeeingChild = false;
	CurrentAlertDelay = 0;
	CurrentState = EEnemyState::Idle;
	ToggleStateVisuals();
	KidController->UpdateAlertState(0);
	KidController->GameOver();
}


void AEnemy::CheckFOVLength()
{
	const FVector Front = GetActorRotation().Vector();
	const FVector Left = GetActorRotation().Vector().RotateAngleAxis(FOVAngle, FVector::UpVector);
	const FVector Right = GetActorRotation().Vector().RotateAngleAxis(-FOVAngle, FVector::UpVector);

	float FrontDistance = CheckInDirection(Front);
	float LeftDistance = CheckInDirection(Left);
	float RightDistance = CheckInDirection(Right);

	float FrontScale = MaxFOVLength;
	float SideScale = MaxFOVLength;

	if (FrontDistance > 0)
	{
		FrontScale = FrontDistance;
	}

	if (LeftDistance > 0 || RightDistance > 0)
	{
		SideScale = RightDistance;

		if ((LeftDistance > 0 && LeftDistance < SideScale) || SideScale <= 0)
		{
			SideScale = LeftDistance;
		}
	}
	
	FVector FOVScale = (FVector::UpVector * FrontScale + FVector::RightVector * SideScale) * FOVScaleFactor + FVector::ForwardVector * FOVHeight;
	FieldOfView->SetWorldScale3D(FOVScale);
	FieldOfView->SetWorldLocation(GetActorLocation() + Front * FrontScale);

	FOVShadow->SetWorldScale3D(FOVScale);
	FOVShadow->SetWorldLocation(GetActorLocation() + Front * FrontScale / 2);
}

float AEnemy::CheckInDirection(FVector Direction)
{
	const FVector StartTrace = GetPawnViewLocation() + Direction;
	const FVector EndTrace = GetPawnViewLocation() + Direction * MaxFOVLength;

	FCollisionQueryParams TraceParams(FName(TEXT("FOVLength")), true, this);
	TraceParams.AddIgnoredActor(Kid);
	TraceParams.AddIgnoredActor(this);

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_WorldStatic, TraceParams);

	return Hit.Distance;
}



void AEnemy::EnteredFieldOfView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor))
		{
			KidController = newCollision->GetKidController();
			bIsSeeingChild = true;
			switch (KidController->GetStealthState())
			{
				case EStealthState::Exposed:
					KillChild();
					break;
				case EStealthState::PartiallyHidden:
					CurrentState = EEnemyState::ActiveAlert;
					ToggleStateVisuals();
					break;
				case EStealthState::TotallyHidden:
					break;
			}
		}
	}
}

void AEnemy::ExitedFieldOfView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor);
		if (newCollision && KidController != nullptr)
		{
			if (CurrentState == EEnemyState::ActiveAlert)
			{
				CurrentState = EEnemyState::PassiveAlert;
				ToggleStateVisuals();
			}
			bIsSeeingChild = false;
		}
	}
}

void AIntermitentEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EEnemyState::Idle)
	{
		if (CurrentActiveDelay < ActiveTime)
		{
			CurrentActiveDelay += DeltaTime;
			if (CurrentActiveDelay >= ActiveTime)
			{
				CurrentInactiveDelay = 0;
				FOVScale = FieldOfView->GetComponentScale();
				FOVShadow->SetVisibility(false);
				FieldOfView->SetCollisionResponseToAllChannels(ECR_Ignore);
				bShouldSleep = true;
			}
		}
		else if (CurrentInactiveDelay < InactiveTime)
		{
			CurrentInactiveDelay += DeltaTime;
			UpdateIndicatorState(CurrentInactiveDelay / InactiveTime);
			if (CurrentInactiveDelay >= InactiveTime)
			{
				CurrentActiveDelay = 0;
				UpdateIndicatorState(0);
				FOVShadow->SetVisibility(true);
				FieldOfView->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				bShouldSleep = false;
			}
		}
	}
}

void ARotatingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EEnemyState::Idle)
	{
		CurrentRotateDelay += DeltaTime;
		if (CurrentRotateDelay >= RotateTime)
		{
			CurrentRotateDelay = 0;
			RotationIndex++;
			if (RotationIndex >= Rotations.Num())
			{
				RotationIndex = 0;
			}
			SetActorRotation(Rotations[RotationIndex]);
		}
	}
}

APatrolingEnemy::APatrolingEnemy()
{
	PatrolRoad = CreateDefaultSubobject<USplineComponent>(TEXT("Patrol Road"));
	PatrolRoad->SetupAttachment(RootComponent);
}

FVector APatrolingEnemy::GetWorldLocationByIndex(int Index) const
{
	if (PatrolRoad && Index < GetLastIndex())
	{
		return PatrolRoad->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
	}
	return GetActorLocation();
}

int APatrolingEnemy::GetLastIndex() const
{
	if (PatrolRoad)
	{
		return PatrolRoad->GetNumberOfSplinePoints();
	}

	return 0;
}

void APatrolingEnemy::BeginPlay()
{
	Super::BeginPlay();
	MoveToLocation(GetWorldLocationByIndex(PatrolIndex));
}

void APatrolingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EEnemyState::Idle)
	{
		if (bReachedDestination)
		{
			if (bIsWalking)
			{
				bIsWalking = false;
				StopMoving();
			}

			InPositionDelay += DeltaTime;
			if (InPositionDelay >= GetCurrentDelay())
			{
				InPositionDelay = 0;
				PatrolIndex++;
				if (PatrolIndex >= GetLastIndex())
				{
					PatrolIndex = 0;
				}
				MoveToLocation(GetWorldLocationByIndex(PatrolIndex));
			}
		}
		else if (!bIsWalking)
		{
			bIsWalking = true;
			MoveToLocation(GetWorldLocationByIndex(PatrolIndex));
		}
	}
	else
	{
		if (bIsWalking)
		{
			StopMoving();
			bIsWalking = false;
		}
	}
}

float APatrolingEnemy::GetCurrentDelay()
{
	if (PatrolIndex < DelayPerStop.Num())
	{
		return DelayPerStop[PatrolIndex];
	}
	return 0;
}

