

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AttributePointAward = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 SpellPointAward = 1;
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Information")
	TArray<FAuraLevelUpInfo> LevelInformation;

	int32 FindLevelForXP(const int32 TotalExperienceAmount) const;
};
