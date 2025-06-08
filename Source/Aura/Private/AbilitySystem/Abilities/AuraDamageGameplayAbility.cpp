


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

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
