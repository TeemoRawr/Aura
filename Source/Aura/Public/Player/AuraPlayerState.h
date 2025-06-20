

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32 /* StateValue */)

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	TObjectPtr<UAttributeSet> GetAttributeSet() const { return AttributeSet; };

	FORCEINLINE int32 GetXP() const { return TotalExperienceAmount; }
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }

	void SetXP(int32 NewTotalExperienceAmount);
	void SetLevel(int32 NewLevel);
	void SetAttributePoints(int32 NewAttributePoints);
	void SetSpellPoints(int32 NewSpellPoints);
	
	void AddXP(int32 ExperienceAmountToAdd);
	void AddToLevel(int32 LevelsToAdd);
	void AddAttributePoints(int32 NewAttributePoints);
	void AddSpellPoints(int32 SpellPointsToAdd);

	FOnPlayerStateChanged OnXPAmountChangedDelegate;
	FOnPlayerStateChanged OnLevelChangedDelegate;
	FOnPlayerStateChanged OnAttributePointsChangedDelegate;
	FOnPlayerStateChanged OnSpellPointsChangedDelegate;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_TotalExperienceAmount)
	int32 TotalExperienceAmount;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
	
	UFUNCTION()
	void OnRep_TotalExperienceAmount(int32 OldTotalExperienceAmount);
};
