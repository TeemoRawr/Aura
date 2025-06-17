


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FAuraAbilityInfo& AuraAbilityInfo : AbilityInformation)
	{
		if (AuraAbilityInfo.AbilityTag == AbilityTag)
		{
			return AuraAbilityInfo;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Ability Tag [%s] not found"), *AbilityTag.ToString());
	}

	return FAuraAbilityInfo();
}
