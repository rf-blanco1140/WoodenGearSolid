#include "ContextInteractable.h"
#include "KidCharacter.h"
#include "KidController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AContextInteractable::AContextInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ActivationRange = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationRange"));
	ActivationRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActivationRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = ActivationRange;
}

// Called when the game starts or when spawned
void AContextInteractable::BeginPlay()
{
	Super::BeginPlay();
	ActivationRange->OnComponentBeginOverlap.AddDynamic(this, &AContextInteractable::OnOverlapRangeBegin);
	ActivationRange->OnComponentEndOverlap.AddDynamic(this, &AContextInteractable::OnOverlapRangeEnd);
	player = nullptr;
}

bool AContextInteractable::CanOverlap()
{
	return true;
}

bool AContextInteractable::CanInteract()
{
	return true;
}

bool AContextInteractable::InteractWith()
{
	return CanInteract();
}

void AContextInteractable::DettachInteraction()
{
	if (player->GetObjectSelected() == this)
	{
		player->ChangeObjectSelected(this);
	}
	kid = nullptr;
	player = nullptr;
	UpdatePlayerInRange(false);
}


void AContextInteractable::FinishInteraction()
{
	ActivationRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	DettachInteraction();
	UpdateFinishedVisuals();
}

FString AContextInteractable::GetPromptText()
{
	return FString("Press button to interact");
}


void AContextInteractable::OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CanOverlap())
		return;

	if (OtherActor)
	{
		if (AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor))
		{
			kid = newCollision;
			player = kid->GetKidController();
			if (player != nullptr)
			{
				player->ChangeObjectSelected(this);
				UpdatePlayerInRange(true);
			}
		}
	}

}

void AContextInteractable::OnOverlapRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!CanOverlap())
		return;

	if (OtherActor)
	{
		AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor);
		if (newCollision && player != nullptr)
		{
			DettachInteraction();
		}
	}
}

bool ACollectableInteractable::CanOverlap()
{
	return !finished;
}

bool ACollectableInteractable::CanInteract()
{
	return !finished;
}

bool ACollectableInteractable::InteractWith()
{
	if (finished)
	{
		return false;
	}

	player->CollectItem(InteractionTag);
	finished = true;
	FinishInteraction();
	return true;
}

FString ACollectableInteractable::GetPromptText()
{
	if (FInteractableType* Item = TagData->FindRow<FInteractableType>(FName(InteractionTag.ToString()), ""))
	{
		return FString("Press button to collect " + Item->ScreenName.ToString());
	}
	return FString("Press button to collect " + InteractionTag.ToString());
}

bool ALockedInteractable::CanOverlap()
{
	return !finished;
}

bool ALockedInteractable::CanInteract()
{
	return !finished && player != nullptr && player->HasCollectedItem(InteractionTag);
}

bool ALockedInteractable::InteractWith()
{
	if (!CanInteract())
	{
		return false;
	}

	finished = true;
	FinishInteraction();
	return true;
}

FString ALockedInteractable::GetPromptText()
{
	if (FInteractableType* Item = TagData->FindRow<FInteractableType>(FName(InteractionTag.ToString()), ""))
	{
		return FString("Use " + Item->ScreenName.ToString() + " to unlock");
	}
	return FString("Use " + InteractionTag.ToString() + " to unlock");
}

AClimbingSurface::AClimbingSurface()
{
	PrimaryActorTick.bCanEverTick = true;

	SurfaceTop = CreateDefaultSubobject<UArrowComponent>(TEXT("Surface top"));
	SurfaceTop->SetupAttachment(RootComponent);

	TopDetectionRange = CreateDefaultSubobject<UBoxComponent>(TEXT("Top Detection Range"));
	TopDetectionRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	TopDetectionRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TopDetectionRange->SetupAttachment(RootComponent);
}

void AClimbingSurface::BeginPlay()
{
	Super::BeginPlay();
	TopDetectionRange->OnComponentBeginOverlap.AddDynamic(this, &AClimbingSurface::OnReachedTop);
}

bool AClimbingSurface::CanInteract()
{
	return kid != nullptr || kid->GetCurrentState() == EKidState::Climbing;
}

bool AClimbingSurface::InteractWith()
{
	if (!CanInteract())
	{
		return false;
	}

	kid->StartClimbing();
	DettachInteraction();
	return true;
}

void AClimbingSurface::OnReachedTop(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor))
		{
			if (newCollision->GetCurrentState() != EKidState::Climbing)
				return;

			newCollision->SetActorLocation(SurfaceTop->GetComponentLocation());
			newCollision->StopClimbing();
		}
	}
}

FString AClimbingSurface::GetPromptText()
{
	return FString("Start climbing");
}