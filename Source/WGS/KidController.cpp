#include "KidController.h"
#include "ContextInteractable.h"

void AKidController::BeginPlay()
{
	/*
	if (IntroHUD_BP)
	{
		IntroHUD = Cast<UIntroStory>(CreateWidget<UUserWidget>(this, IntroHUD_BP));
		IntroHUD->AddToViewport();
		if (startWithIntro)
			IntroHUD->SetVisibility(ESlateVisibility::Visible);
		else
			IntroHUD->SetVisibility(ESlateVisibility::Hidden);
	}
	*/
	if (PromptHUD_BP)
	{
		PromptHUD = Cast<UInteractionPrompt>(CreateWidget<UUserWidget>(this, PromptHUD_BP));
		PromptHUD->AddToViewport();
		PromptHUD->SetVisibility(ESlateVisibility::Visible);
		PromptHUD->SetDescriptionText(FString(""));
	}
}

bool AKidController::CanMove()
{
	/*
	if (PauseHUD && PauseHUD->IsVisible())
		return false;

	if (EndingHUD && EndingHUD->IsVisible())
		return false;

	if (IntroHUD->IsVisible())
		return false;

	*/
	return true;
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
	if (CurrentInteractable != nullptr)
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

void UInteractionPrompt::SetDescriptionText(FString newText, bool bCanInteract)
{
	PromptChanged(newText, bCanInteract);
}
