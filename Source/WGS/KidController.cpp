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
	if (InventoryHUD_BP)
	{
		InventoryHUD = Cast<UInventoryScreen>(CreateWidget<UUserWidget>(this, InventoryHUD_BP));
		InventoryHUD->AddToViewport();
		InventoryHUD->SetVisibility(ESlateVisibility::Visible);
	}
	if (StateHUD_BP)
	{
		StateHUD = Cast<UKidStateUI>(CreateWidget<UUserWidget>(this, StateHUD_BP));
		StateHUD->AddToViewport();
		StateHUD->SetVisibility(ESlateVisibility::Visible);
		UpdateAlertState(0);
		StateHUD->StealthStateChanged(GetStealthState());
	}

}

void AKidController::UpdateAlertState(float Percentage)
{
	StateHUD->UpdateAlertIndicator(Percentage);
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

void AKidController::CollectItem(FGameplayTag& Item)
{
	if (!CollectedKeys.Contains(Item))
	{
		CollectedKeys.Add(Item, 1);
	}
	else
	{
		CollectedKeys[Item]++;
	}

	InventoryHUD->UpdateInventory(CollectedKeys);
}

bool AKidController::HasCollectedItem(FGameplayTag& Item, int Quantity)
{
	return CollectedKeys.Contains(Item) && CollectedKeys[Item] >= Quantity;
}

void AKidController::ConsumeItem(FGameplayTag& Item, int Quantity)
{
	if (CollectedKeys.Contains(Item))
	{
		CollectedKeys[Item] -= Quantity;
		if (CollectedKeys[Item] <= 0)
		{
			CollectedKeys.Remove(Item);
		}
		InventoryHUD->UpdateInventory(CollectedKeys);
	}
}


void AKidController::ToggleHiddingSpot(AHidingSpot* NewHidingSpot)
{
	if (HidingSpots.Contains(NewHidingSpot))
	{
		HidingSpots.Remove(NewHidingSpot);
	}
	else 
	{
		HidingSpots.Add(NewHidingSpot);
	}
	NewHidingSpot->UpdateHidingVisuals();
	StateHUD->StealthStateChanged(GetStealthState());
}

EStealthState AKidController::GetStealthState() const
{
	EStealthState Best = EStealthState::Exposed;
	for (AHidingSpot* Spot : HidingSpots)
	{
		if (Spot->GetHidingLevel() == EStealthState::TotallyHidden)
		{
			Best = EStealthState::TotallyHidden;
		}
		else if (Spot->GetHidingLevel() == EStealthState::PartiallyHidden && Best == EStealthState::Exposed)
		{
			Best = EStealthState::PartiallyHidden;
		}
	}

	return Best;
}

TArray<AHidingSpot*> AKidController::GetCurrentHidingSpots() const
{
	return HidingSpots;
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

void UInventoryScreen::UpdateInventory(TMap<FGameplayTag, int>& OwnedKeys)
{
	int Index = 0;
	for (auto ItemKey : OwnedKeys)
	{
		if (Index >= Items.Num())
		{
			Items[Index]->Toggle(false);
		}
		else
		{
			Items[Index]->Toggle(true);
			Items[Index]->UpdateItem(GetIcon(ItemKey.Key), GetDisplayName(ItemKey.Key), ItemKey.Value);
		}
		Index++;
	}

	while (Index < Items.Num())
	{
		Items[Index]->Toggle(false);
		Index++;
	}
}

UTexture2D* UInventoryScreen::GetIcon(FGameplayTag& KeyTag)
{
	if (FInteractableType* Item = TagData->FindRow<FInteractableType>(FName(KeyTag.ToString()), ""))
	{
		return Item->Icon;
	}
	return nullptr;
}

FName UInventoryScreen::GetDisplayName(FGameplayTag& KeyTag)
{
	if (FInteractableType* Item = TagData->FindRow<FInteractableType>(FName(KeyTag.ToString()), ""))
	{
		return Item->ScreenName;
	}
	return FName("Undefined");
}

