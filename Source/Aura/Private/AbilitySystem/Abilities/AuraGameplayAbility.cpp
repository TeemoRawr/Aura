


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - Lerem Impsum", Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level</><Level>%d</>"), Level);
}

FString UAuraGameplayAbility::GetLockDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell locked until %d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(const int32 InLevel) const
{
	float ManaCost = 0.f;
	
	if (UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (const FGameplayModifierInfo& Modifier : CostEffect->Modifiers)
		{
			if (Modifier.Attribute == UAuraAttributeSet::GetManaAttribute())
			{				
				Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break;
			}
		}
	}

	return -ManaCost;
}

float UAuraGameplayAbility::GetCooldown(const int32 InLevel) const
{
	float Cooldown = 0.f;

	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}

	return Cooldown;
}
