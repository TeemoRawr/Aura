


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);

	for (TPair<FGameplayTag, FScalableFloat> DamageType : DamageTypes)
	{
		float DamageMagnitude = DamageType.Value.GetValueAtLevel(GetAbilityLevel());
		
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			DamageSpecHandle,
			DamageType.Key,
			DamageMagnitude);
	}
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}

float UAuraDamageGameplayAbility::GetDamageByDamageType(const int32 InLevel, const FGameplayTag& DamageType)
{
	checkf(DamageTypes.Contains(DamageType), TEXT("DamageType &s not found"), *DamageType.ToString());
	const float Damage = DamageTypes[DamageType].GetValueAtLevel(InLevel);

	return Damage;
}
