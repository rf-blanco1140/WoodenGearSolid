#include "Enemy.h"
#include "KidCharacter.h"
#include "KidController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

AEnemy::AEnemy()
{
	FieldOfView = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Field Of View"));
	FieldOfView->SetCollisionResponseToAllChannels(ECR_Ignore);
	FieldOfView->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FieldOfView->SetupAttachment(RootComponent);

	ActionableRange = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Actionable Range"));
	ActionableRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActionableRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ActionableRange->SetupAttachment(RootComponent);

	PersonalSpace = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Personal Space"));
	PersonalSpace->SetCollisionResponseToAllChannels(ECR_Ignore);
	PersonalSpace->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PersonalSpace->SetupAttachment(RootComponent);

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EEnemyState::Idle;
	FieldOfView->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::EnteredFieldOfView);
	FieldOfView->OnComponentEndOverlap.AddDynamic(this, &AEnemy::ExitedFieldOfView);
	ActionableRange->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::EnteredCatchingRange);
	PersonalSpace->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::EnteredCatchingRange);
	UpdateAlertState(0);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EEnemyState::ActiveAlert)
	{
		CurrentAlertDelay += DeltaTime;
		UpdateAlertState(CurrentAlertDelay / AlertTime);
		if (CurrentAlertDelay >= AlertTime)
		{
			KidController->GameOver();
		}
	}
	else if (CurrentState == EEnemyState::PassiveAlert)
	{
		CurrentAlertDelay -= DeltaTime;
		UpdateAlertState(CurrentAlertDelay / AlertTime);
		if (CurrentAlertDelay <= 0)
		{
			CurrentState = EEnemyState::Idle;
			ToggleStateVisuals();
		}
	}
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
			KidController = nullptr;
		}
	}
}

void AEnemy::EnteredCatchingRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor))
		{
			newCollision->GetKidController()->GameOver();
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
