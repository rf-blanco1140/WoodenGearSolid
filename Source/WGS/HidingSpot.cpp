#include "HidingSpot.h"
#include "KidCharacter.h"
#include "KidController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
AHidingSpot::AHidingSpot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HiddingArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Hidding Area"));
	HiddingArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	HiddingArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = HiddingArea;
}

// Called when the game starts or when spawned
void AHidingSpot::BeginPlay()
{
	Super::BeginPlay();
	HiddingArea->OnComponentBeginOverlap.AddDynamic(this, &AHidingSpot::OnHidingBegin);
	HiddingArea->OnComponentEndOverlap.AddDynamic(this, &AHidingSpot::OnHidingEnd);
	player = nullptr;
}

void AHidingSpot::OnHidingBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor))
		{
			player = newCollision->GetKidController();
			if (player != nullptr)
			{
				player->ToggleHiddingSpot(this, true);
				UpdateHidingVisuals();
			}
		}
	}
}

void AHidingSpot::OnHidingEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor);
		if (newCollision && player != nullptr)
		{
			player->ToggleHiddingSpot(this, false);
			player = nullptr;
			UpdateHidingVisuals();
		}
	}
}

EStealthState AHidingSpot::GetHidingLevel()
{
	return TypeOfHiding;
}

bool AHidingSpot::IsHidingPlayer()
{
	return player != nullptr && player->GetCurrentHidingSpots().Contains(this);
}
