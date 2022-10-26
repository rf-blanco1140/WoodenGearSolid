#include "KidController.h"
#include "ContextInteractable.h"

void AKidController::BeginPlay()
{
	bIsPlaying = true;
	if (PromptHUD_BP)
	{
		PromptHUD = Cast<UInteractionPrompt>(CreateWidget<UUserWidget>(this, PromptHUD_BP));
		PromptHUD->AddToViewport();
		PromptHUD->SetVisibility(ESlateVisibility::Visible);
		PromptHUD->SetDescriptionText(FString(""));
	}
	if (GameOverHUD_BP)
	{
		GameOverHUD = CreateWidget<UUserWidget>(this, GameOverHUD_BP);
		GameOverHUD->AddToViewport();
		GameOverHUD->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool AKidController::CanMove()
{
	return bIsPlaying;
}

void AKidController::ChangeObjectSelected(AContextInteractable* Interactable)
{
	if (CurrentInteractable == nullptr || CurrentInteractable != Interactable)
	{
		CurrentInteractable = Interactable;
		PromptHUD->SetDescriptionText(CurrentInteractable->GetPromptText(), CurrentInteractable->CanInteract());
	}
	else
	{
		CurrentInteractable = nullptr;
		PromptHUD->SetDescriptionText(FString(""));
	}
}

void AKidController::InteractWithSelected()
{
	if (bIsPlaying && CurrentInteractable != nullptr)
	{
		CurrentInteractable->InteractWith();
	}
}

void AKidController::CollectItem(FGameplayTag Item)
{
	CollectedKeys.Add(Item);
}

bool AKidController::HasCollectedItem(FGameplayTag Item)
{
	return CollectedKeys.Contains(Item);
}

void AKidController::ToggleHiddingSpot(AHidingSpot* NewHidingSpot)
{
	if (HidingSpot == nullptr)
	{
		HidingSpot = NewHidingSpot;
	}
	else if (NewHidingSpot == HidingSpot)
	{
		HidingSpot = nullptr;
	}
	else
	{
		AHidingSpot* OldHidingSpot = HidingSpot;
		HidingSpot = NewHidingSpot;
		OldHidingSpot->UpdateHidingVisuals();
	}
}

EStealthState AKidController::GetStealthState() const
{
	if (HidingSpot == nullptr)
	{
		return EStealthState::Exposed;
	}
	else
	{
		return HidingSpot->GetHidingLevel();
	}
}

AHidingSpot* AKidController::GetCurrentHidingSpot() const
{
	return HidingSpot;
}

void AKidController::GameOver()
{
	bIsPlaying = false;
	GameOverHUD->SetVisibility(ESlateVisibility::Visible);
}

void UInteractionPrompt::SetDescriptionText(FString newText, bool bCanInteract)
{
	PromptChanged(newText, bCanInteract);
}
