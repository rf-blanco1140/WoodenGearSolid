#include "Enemy.h"
#include "KidCharacter.h"
#include "KidController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

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
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EEnemyState::Idle;
	FieldOfView->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::EnteredFieldOfView);
	FieldOfView->OnComponentEndOverlap.AddDynamic(this, &AEnemy::ExitedFieldOfView);
	Kid = Cast<AKidCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
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
	
	if (CurrentState == EEnemyState::ActiveAlert)
	{
		CurrentAlertDelay += DeltaTime;
		if (KidController)
		{
			KidController->UpdateAlertState(CurrentAlertDelay / AlertTime);
		}
		if (CurrentAlertDelay >= AlertTime)
		{
			KidController->GameOver();
		}
	}
	else if (CurrentState == EEnemyState::PassiveAlert)
	{
		CurrentAlertDelay -= DeltaTime;
		if (KidController)
		{
			KidController->UpdateAlertState(CurrentAlertDelay / AlertTime);
		}
		if (CurrentAlertDelay <= 0)
		{
			CurrentState = EEnemyState::Idle;
			ToggleStateVisuals();
		}
	}
}

void AEnemy::CheckFOVLength()
{
	const FVector Direction = GetActorRotation().Vector();
	const FVector StartTrace = GetPawnViewLocation() + Direction;
	const FVector EndTrace = GetPawnViewLocation() + Direction * MaxFOVLength;

	FCollisionQueryParams TraceParams(FName(TEXT("FOVLength")), true, this);
	TraceParams.AddIgnoredActor(Kid);
	TraceParams.AddIgnoredActor(this);

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_WorldStatic, TraceParams);

	FVector FOVScale = FieldOfView->GetComponentScale();
	FVector FOVLoc;
	if (Hit.Distance > 0)
	{
		FOVScale.Y = Hit.Distance * FOVScaleFactor;
		FOVLoc = Direction * Hit.Distance;
	}
	else
	{
		FOVScale.Y = MaxFOVLength * FOVScaleFactor;
		FOVLoc = Direction * MaxFOVLength;
	}
	FieldOfView->SetWorldScale3D(FOVScale);
	FieldOfView->SetWorldLocation(GetActorLocation() + FOVLoc);

	FOVShadow->SetWorldScale3D(FieldOfView->GetComponentScale());
	FOVShadow->SetWorldLocation(FieldOfView->GetComponentLocation() - (Direction * MaxFOVLength / 2));
}

void AEnemy::EnteredFieldOfView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor))
		{
			KidController = newCollision->GetKidController();
			switch (KidController->GetStealthState())
			{
				case EStealthState::Exposed:
					KidController->GameOver();
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
			CurrentState = EEnemyState::PassiveAlert;
			ToggleStateVisuals();
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
			}
		}
		else if (CurrentInactiveDelay < InactiveTime)
		{
			CurrentInactiveDelay += DeltaTime;
			if (CurrentInactiveDelay >= InactiveTime)
			{
				CurrentActiveDelay = 0;
				FOVShadow->SetVisibility(true);
				FieldOfView->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
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

