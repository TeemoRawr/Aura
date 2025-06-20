

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(const int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(const int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(const int32& InXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(const int32& InLevel);

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(const int32& InAttributePoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(const int32& InSpellPoints);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;
	
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
};
