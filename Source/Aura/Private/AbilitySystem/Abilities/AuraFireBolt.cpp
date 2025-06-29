


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const float Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	if (Level == 1)
	{
		return FString::Printf(TEXT(
				"<Title>FIRE BOLT</>\n\n"
				"<Default>Lunches a bolt of ire exploding on impact and dealing </><Damage>%i</><Default> fire damage with a chance to burn</>\n\n"
				"<Small>Level: </><Level>%d</>\n"
				"<Small>Mana cost: </><ManaCost>%.1f</>\n"
				"<Small>Cooldown: </><Cooldown>%.1f</>\n"),
			static_cast<int>(Damage), Level, ManaCost, Cooldown);
	}

	return FString::Printf(TEXT(
		"<Title>FIRE BOLT</>\n\n"
		"<Default>Lunches %d bolts of ire exploding on impact and dealing </><Damage>%d</><Default> fire damage with a chance to burn</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"),
		FMath::Min(Level, NumberOfProjectiles), static_cast<int>(Damage), Level, ManaCost, Cooldown);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const float Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		"<Title>FIRE BOLT</>\n\n"
		"<Default>Lunches %d bolts of ire exploding on impact and dealing </><Damage>%i</><Default> fire damage with a chance to burn</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>"),
		FMath::Min(Level, NumberOfProjectiles), static_cast<int>(Damage), Level, ManaCost, Cooldown);
}