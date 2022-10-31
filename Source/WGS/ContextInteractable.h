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
	void OnOverlapRangeBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
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
};

UCLASS()
class WGS_API AClimbingSurface : public AContextInteractable
{
	GENERATED_BODY()

public:
	AClimbingSurface();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UArrowComponent* SurfaceTop;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* TopDetectionRange;

	void BeginPlay() override;

	bool CanInteract() override;
	bool InteractWith() override;
	FString GetPromptText() override;
	UFUNCTION()
	void OnReachedTop(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

