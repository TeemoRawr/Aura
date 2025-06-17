


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(const int32 TotalExperienceAmount) const
{
	int32 Level = 1;
	bool bSearching = true;

	while (bSearching)
	{
		if (LevelInformation.Num() - 1 <= Level) return Level;

		if (TotalExperienceAmount >= LevelInformation[Level].LevelUpRequirement)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}
	}

	return Level;
}
