#include "ContextInteractable.h"
#include "KidCharacter.h"
#include "KidController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

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

bool AContextInteractable::IsFinished()
{
	return finished;
}

bool AContextInteractable::CanInteract()
{
	return true;
}

bool AContextInteractable::InteractWith()
{
	return CanInteract();
	//InteractionFinished.Broadcast();
}

void AContextInteractable::FinishInteraction()
{
	finished = true;
	ActivationRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	UpdateFinishedVisuals();
	UpdatePlayerInRange(false);
}

FString AContextInteractable::GetPromptText()
{
	return FString("Press button to interact");
}


void AContextInteractable::OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (finished)
		return;

	if (OtherActor)
	{
		if (AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor))
		{
			player = newCollision->GetKidController();
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
	if (OtherActor)
	{
		AKidCharacter* newCollision = Cast<AKidCharacter>(OtherActor);
		if (newCollision && player != nullptr && (!finished))
		{
			//player->PlayerCharacterController->ToggleInteractPrompt(false);
			player->ChangeObjectSelected(this);
			player = nullptr;
			UpdatePlayerInRange(false);
		}
	}
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

bool ALockedInteractable::CanInteract()
{
	return !finished && player->HasCollectedItem(InteractionTag);
}

bool ALockedInteractable::InteractWith()
{
	if (!CanInteract())
	{
		return false;
	}

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