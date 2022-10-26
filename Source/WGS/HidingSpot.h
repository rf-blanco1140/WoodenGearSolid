#pragma once

#include <CoreMinimal.h>
#include "GameFramework/Actor.h"
#include "HidingSpot.generated.h"

UENUM(BlueprintType)
enum class EStealthState : uint8
{
	Exposed,
	PartiallyHidden,
	TotallyHidden,
};

UCLASS()
class WGS_API AHidingSpot : public AActor
{
	GENERATED_BODY()

public:
	AHidingSpot();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	EStealthState TypeOfHiding = EStealthState::PartiallyHidden;
	UPROPERTY()
	class AKidController* player;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* HiddingArea;

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EStealthState GetHidingLevel();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsHidingPlayer();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHidingVisuals();

	UFUNCTION()
	void OnHidingBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnHidingEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};