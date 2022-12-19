#pragma once

#include <CoreMinimal.h>
#include "GameFramework/PlayerController.h"
#include "GameplayTagsManager.h"
#include "HidingSpot.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "KidController.generated.h"

class AContextInteractable;
class USoundCue;

UCLASS()
class WGS_API AKidController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class UInteractionPrompt* PromptHUD;
	UPROPERTY()
	class UInventoryScreen* InventoryHUD;
	UPROPERTY()
	UUserWidget* GameOverHUD;
	UPROPERTY()
	UKidStateUI* StateHUD;
	UPROPERTY()
	TMap<FGameplayTag, int> CollectedKeys;
	UPROPERTY()
	AContextInteractable* CurrentInteractable;
	UPROPERTY()
	TArray<AHidingSpot*> HidingSpots;
	UPROPERTY()
	bool bIsPlaying;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadWrite)
	bool bGamePaused;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<UInteractionPrompt> PromptHUD_BP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<UInventoryScreen> InventoryHUD_BP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<UUserWidget> GameOverHUD_BP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<UKidStateUI> StateHUD_BP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundFX)
	USoundCue* GettingCaught;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundFX)
	USoundCue* GrabbedItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundFX)
	USoundCue* UsedItem;
	UPROPERTY()
	class ALevelCheckpoint* CurrentCheckpoint;
	bool CanMove();
	AContextInteractable* GetObjectSelected() const;
	void ChangeObjectSelected(AContextInteractable*);
	void InteractWithSelected();
	void CollectItem(FGameplayTag&);
	bool HasCollectedItem(FGameplayTag&, int Quantity = 1);
	void ConsumeItem(FGameplayTag&, int Quantity = 1);

	void ToggleHiddingSpot(AHidingSpot*);
	EStealthState GetStealthState() const;
	TArray<AHidingSpot*> GetCurrentHidingSpots() const;
	void GameOver();
	void UpdateAlertState(float Percentage);
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

UCLASS()
class WGS_API UKidStateUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StealthStateChanged(const EStealthState StealthState);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateAlertIndicator(const float Percentage);
};

UCLASS()
class WGS_API UInventoryScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	class UDataTable* TagData;
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TArray<UInventoryItem*> Items;

	void UpdateInventory(TMap<FGameplayTag, int>&);
	UTexture2D* GetIcon(FGameplayTag& KeyTag);
	FName GetDisplayName(FGameplayTag& KeyTag);
};

UCLASS()
class WGS_API UInventoryItem : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateItem(UTexture2D* Icon, FName KeyTag, int Quantity);
	UFUNCTION(BlueprintImplementableEvent)
	void Toggle(bool visible);
};