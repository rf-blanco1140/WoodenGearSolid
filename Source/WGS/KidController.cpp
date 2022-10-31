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
		PromptHUD->StealthStateChanged(GetStealthState());
	}
	if (GameOverHUD_BP)
	{
		GameOverHUD = CreateWidget<UUserWidget>(this, GameOverHUD_BP);
		GameOverHUD->AddToViewport();
		GameOverHUD->SetVisibility(ESlateVisibility::Hidden);
	}
	if (InventoryHUD_BP)
	{
		InventoryHUD = Cast<UInventoryScreen>(CreateWidget<UUserWidget>(this, InventoryHUD_BP));
		InventoryHUD->AddToViewport();
		InventoryHUD->SetVisibility(ESlateVisibility::Visible);
	}
}

bool AKidController::CanMove()
{
	return bIsPlaying;
}

AContextInteractable* AKidController::GetObjectSelected() const
{
	return CurrentInteractable;
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
	InventoryHUD->UpdateInventory(CollectedKeys);
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
	PromptHUD->StealthStateChanged(GetStealthState());
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

void UInventoryScreen::UpdateInventory(TArray<FGameplayTag>& OwnedKeys)
{
	int Index = 0;
	for (FGameplayTag ItemKey : OwnedKeys)
	{
		if (Index >= Items.Num() || !ItemIcons.Contains(ItemKey))
			return;

		Items[Index]->UpdateItem(ItemIcons[ItemKey], ItemKey);
		Index++;
	}
}

