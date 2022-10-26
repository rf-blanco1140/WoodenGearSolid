#pragma once

#include <CoreMinimal.h>
#include "GameFramework/PlayerController.h"
#include "GameplayTagsManager.h"
#include "HidingSpot.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "KidController.generated.h"

class AContextInteractable;

UCLASS()
class WGS_API AKidController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class UInteractionPrompt* PromptHUD;
	UPROPERTY()
	UUserWidget* GameOverHUD;
	UPROPERTY()
	TArray<FGameplayTag> CollectedKeys;
	UPROPERTY()
	AContextInteractable* CurrentInteractable;
	UPROPERTY()
	AHidingSpot* HidingSpot;
	UPROPERTY()
	bool bIsPlaying;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<UInteractionPrompt> PromptHUD_BP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<UUserWidget> GameOverHUD_BP;
	bool CanMove();
	void ChangeObjectSelected(AContextInteractable*);
	void InteractWithSelected();
	void CollectItem(FGameplayTag);
	bool HasCollectedItem(FGameplayTag);

	void ToggleHiddingSpot(AHidingSpot*);
	EStealthState GetStealthState() const;
	AHidingSpot* GetCurrentHidingSpot() const;
	void GameOver();
};

UCLASS()
class WGS_API UInteractionPrompt : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDescriptionText(FString newText, bool bCanInteract = false);
	UFUNCTION(BlueprintImplementableEvent)
	void PromptChanged(const FString& newText, bool bCanInteract);
};