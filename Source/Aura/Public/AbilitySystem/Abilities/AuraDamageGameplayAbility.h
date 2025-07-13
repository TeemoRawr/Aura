

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly);
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FGameplayTag  DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DeathImpulseMagnitude = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
	float DebuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Knockback")
	float KnockbackChance = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Knockback")
	float KnockbackForceMagnitude = 1000.f;
};
