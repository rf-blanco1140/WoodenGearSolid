#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

AEnemy::AEnemy()
{
	FieldOfView = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Field Of View"));
	FieldOfView->SetRelativeLocation(FVector(0.0f));
	FieldOfView->SetCollisionResponseToAllChannels(ECR_Ignore);
	FieldOfView->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ActionableRange = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Actionable Range"));
	ActionableRange->SetRelativeLocation(FVector(0.0f));
	ActionableRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActionableRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PersonalSpace = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Personal Space"));
	PersonalSpace->SetRelativeLocation(FVector(0.0f));
	PersonalSpace->SetCollisionResponseToAllChannels(ECR_Ignore);
	PersonalSpace->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EEnemyState::Idle;
	FieldOfView->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::EnteredFieldOfView);
	ActionableRange->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::EnteredCatchingRange);
	PersonalSpace->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::EnteredCatchingRange);
}

void AEnemy::EnteredFieldOfView(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AEnemy::EnteredCatchingRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
