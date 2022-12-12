#pragma once

#include <CoreMinimal.h>
#include "GameFramework/Actor.h"
#include "GameplayTagsManager.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "ContextInteractable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionFinished);

USTRUCT(BlueprintType)
struct WGS_API FInteractableType : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InteractionTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ScreenName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ConsumedOnUse;
};

UCLASS()
class WGS_API AContextInteractable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AContextInteractable();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* ActivationRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UDataTable* TagData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		class AKidCharacter* kid;
	UPROPERTY()
		class AKidController* player;

public:
	virtual bool CanOverlap();
	virtual bool CanInteract();
	virtual bool InteractWith();
	virtual void DettachInteraction();
	virtual void KidEnteredRange(AKidCharacter*);
	virtual void KidExitedRange(AKidCharacter*);
	void FinishInteraction();
	virtual FString GetPromptText();
	bool IsFinished();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePlayerInRange(bool InRange);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateFinishedVisuals();

	UPROPERTY(BlueprintAssignable)
		FInteractionFinished InteractionFinished;

	UFUNCTION()
		virtual void OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

UCLASS()
class WGS_API ACollectableInteractable : public AContextInteractable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interaction")
		FGameplayTag InteractionTag;
	UPROPERTY(BlueprintReadOnly)
		bool finished;

	bool CanOverlap() override;
	bool CanInteract() override;
	bool InteractWith() override;
	FString GetPromptText() override;
};

UCLASS()
class WGS_API ALockedInteractable : public AContextInteractable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interaction")
		FGameplayTag InteractionTag;
	UPROPERTY(BlueprintReadOnly)
		bool finished;

	bool CanOverlap() override;
	bool CanInteract() override;
	bool InteractWith() override;
	FString GetPromptText() override;
	virtual void DeductPlayerInventory();
};

UCLASS()
class WGS_API AMultipleKeyLockedInteractable : public ALockedInteractable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	FName PromptText;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	TMap<FGameplayTag, int> InteractionTags;

	bool CanInteract() override;
	FString GetPromptText() override;
	void DeductPlayerInventory() override;
};

UCLASS()
class WGS_API AClimbingSurface : public AContextInteractable
{
	GENERATED_BODY()

public:
	AClimbingSurface();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UArrowComponent* SurfaceTop;

	bool CanInteract() override;
	bool InteractWith() override;
	FString GetPromptText() override;

protected:
	float GetClimbingPercentage();
	void Tick(float DeltaTime) override;
	float GetClimbHeight();
	float GetClimbStart();
};

UCLASS()
class WGS_API ALevelDoor : public AContextInteractable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelPromptName;

	bool InteractWith() override;
	FString GetPromptText() override;
};

UCLASS()
class WGS_API ALevelCheckpoint : public AContextInteractable
{
	GENERATED_BODY()

public:
	
	bool InteractWith() override;
	FString GetPromptText() override;
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointSelected();
};

