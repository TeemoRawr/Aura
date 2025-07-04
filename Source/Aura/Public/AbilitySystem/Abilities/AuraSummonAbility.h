

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();
	
	UFUNCTION(BlueprintPure, Category = "Summoning")
	TSubclassOf<APawn> GetRandomMinionClass();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	float MinSpawnDistance = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	float MaxSpawnDistance = 450.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Summoning")
	float SpawnSpread = 90.f;
};
