#include "ContextInteractable.h"
#include "KidCharacter.h"
#include "KidController.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
AContextInteractable::AContextInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ActivationRange = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationRange"));
	ActivationRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActivationRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = ActivationRange;

	Outline = CreateDefaultSubobject<UDecalComponent>(TEXT("Outline"));
	Outline->SetupAttachment(ActivationRange);
}

// Called when the game starts or when spawned
void AContextInteractable::BeginPlay()
{
	Super::BeginPlay();
	ActivationRange->OnComponentBeginOverlap.AddDynamic(this, &AContextInteractable::OnOverlapRangeBegin);
	ActivationRange->OnComponentEndOverlap.AddDynamic(this, &AContextInteractable::OnOverlapRangeEnd);
	player = nullptr;
	Outline->DecalSize = ActivationRange->GetUnscaledBoxExtent();
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
	if (player != nullptr && player->GetObjectSelected() == this)
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

void AContextInteractable::KidEnteredRange(AKidCharacter* newCollision)
{
	kid = newCollision;
	player = kid->GetKidController();
	if (player != nullptr)
	{
		player->ChangeObjectSelected(this);
		UpdatePlayerInRange(true);
	}
}

void AContextInteractable::KidExitedRange(AKidCharacter* newCollision)
{
	if (player != nullptr)
	{
		DettachInteraction();
	}
}


void AContextInteractable::OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CanOverlap())
		return;

	if (OtherActor)
	{
		if (AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor))
		{
			KidEnteredRange(newCollision);
		}
	}

}

void AContextInteractable::OnOverlapRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!CanOverlap())
		return;

	if (OtherActor)
	{
		if (AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor))
		{
			KidExitedRange(newCollision);
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
		return FString("Collect " + Item->ScreenName.ToString());
	}
	return FString("Collect " + InteractionTag.ToString());
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
	if (UnlockedFX)
	{
		UGameplayStatics::PlaySound2D(this, UnlockedFX);
	}

	finished = true;
	DeductPlayerInventory();
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

void ALockedInteractable::DeductPlayerInventory()
{
	if (FInteractableType* Item = TagData->FindRow<FInteractableType>(FName(InteractionTag.ToString()), ""))
	{
		if (Item->ConsumedOnUse)
		{
			player->ConsumeItem(InteractionTag);
		}
	}
}

bool AMultipleKeyLockedInteractable::CanInteract()
{
	if (!finished)
	{
		if (player != nullptr)
		{
			for (auto Requirement : InteractionTags)
			{
				if (!player->HasCollectedItem(Requirement.Key, Requirement.Value))
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

FString AMultipleKeyLockedInteractable::GetPromptText()
{
	auto InterTag = InteractionTags.begin();
	if (FInteractableType* Item = TagData->FindRow<FInteractableType>(InterTag->Key.GetTagName(), ""))
	{
		return FString("Use " + Item->ScreenName.ToString() + " (" + FString::FromInt(InterTag->Value) + ") to unlock");
	}
	return FString("Use " + InteractionTag.ToString() + " to unlock");
}

void AMultipleKeyLockedInteractable::DeductPlayerInventory()
{
	for (auto Requirement : InteractionTags)
	{
		if (FInteractableType* Item = TagData->FindRow<FInteractableType>(FName(Requirement.Key.ToString()), ""))
		{
			if (Item->ConsumedOnUse)
			{
				player->ConsumeItem(Requirement.Key, Requirement.Value);
			}
		}
	}
}

AClimbingSurface::AClimbingSurface()
{
	PrimaryActorTick.bCanEverTick = true;

	SurfaceTop = CreateDefaultSubobject<UArrowComponent>(TEXT("Surface top"));
	SurfaceTop->SetupAttachment(RootComponent);
}

bool AClimbingSurface::CanInteract()
{
	return kid != nullptr && kid->GetCurrentState() != EKidState::Climbing;
}

bool AClimbingSurface::InteractWith()
{
	if (!CanInteract())
	{
		return false;
	}

	FVector StartLocation = FVector(ActivationRange->GetComponentLocation().X, ActivationRange->GetComponentLocation().Y, GetClimbHeight() * 0.01 + GetClimbStart() + kid->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	if (kid->GetActorLocation().Z > ActivationRange->GetComponentLocation().Z)
	{
		StartLocation.Z = GetClimbHeight() * 0.99 + GetClimbStart() - kid->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	kid->SetActorLocation(StartLocation);
	kid->StartClimbing();
	return true;
}

float AClimbingSurface::GetClimbingPercentage()
{
	float PreFeetPercentage = (kid->GetActorLocation().Z - GetClimbStart()) / GetClimbHeight();
	if (PreFeetPercentage < 0.5)
	{
		return (kid->GetActorLocation().Z - GetClimbStart() - kid->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) / GetClimbHeight();
	}
	return (kid->GetActorLocation().Z - GetClimbStart() + kid->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) / GetClimbHeight();
}

void AClimbingSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (kid != nullptr && kid->GetCurrentState() == EKidState::Climbing)
	{
		if (GetClimbingPercentage() >= 1)
		{
			AKidCharacter* KidPtr = kid;
			KidPtr->SetActorLocation(SurfaceTop->GetComponentLocation());
			KidPtr->StopClimbing();
			DettachInteraction();
		}
		else if (GetClimbingPercentage() <= 0)
		{
			AKidCharacter* KidPtr = kid;
			KidPtr->SetActorLocation(FVector(ActivationRange->GetComponentLocation().X, ActivationRange->GetComponentLocation().Y, GetClimbStart()));
			KidPtr->StopClimbing();
			DettachInteraction();
		}
	}
}

float AClimbingSurface::GetClimbHeight()
{
	return ActivationRange->GetUnscaledBoxExtent().Z * 2;
}

float AClimbingSurface::GetClimbStart()
{
	return GetActorLocation().Z - GetClimbHeight() / 2;
}

FString AClimbingSurface::GetPromptText()
{
	return FString("Start climbing");
}

bool ALevelDoor::InteractWith()
{
	if (!CanInteract())
	{
		return false;
	}

	if (UWorld* World = GetWorld())
	{
		FString CurrentLevel = World->GetMapName();
		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			kid->GetKidController()->CurrentCheckpoint = nullptr;
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
	return true;
}

FString ALevelDoor::GetPromptText()
{
	return FString("Go to " + LevelPromptName.ToString());
}

bool ALevelCheckpoint::InteractWith()
{
	if (!CanInteract())
	{
		return false;
	}

	if (kid)
	{
		kid->GetKidController()->CurrentCheckpoint = this;
		CheckpointSelected();
	}
	return true;
}

FString ALevelCheckpoint::GetPromptText()
{
	return FString("Activate this checkpoint");
}
