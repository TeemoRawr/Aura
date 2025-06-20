


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	SetNetUpdateFrequency(100.f);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, TotalExperienceAmount);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::SetXP(int32 NewTotalExperienceAmount)
{
	TotalExperienceAmount = NewTotalExperienceAmount;
	OnXPAmountChangedDelegate.Broadcast(TotalExperienceAmount);
}

void AAuraPlayerState::SetLevel(int32 NewLevel)
{
	Level = NewLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::SetAttributePoints(int32 NewAttributePoints)
{
	AttributePoints = NewAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetSpellPoints(int32 NewSpellPoints)
{
	SpellPoints = NewSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddAttributePoints(int32 NewAttributePoints)
{
	AttributePoints += NewAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddSpellPoints(int32 SpellPointsToAdd)
{
	SpellPointsToAdd += SpellPointsToAdd;
	OnSpellPointsChangedDelegate.Broadcast(SpellPointsToAdd);
}

void AAuraPlayerState::AddToLevel(int32 LevelsToAdd)
{
	Level += LevelsToAdd;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddXP(int32 ExperienceAmountToAdd)
{
	TotalExperienceAmount += ExperienceAmountToAdd;
	OnXPAmountChangedDelegate.Broadcast(TotalExperienceAmount);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::OnRep_TotalExperienceAmount(int32 OldTotalExperienceAmount)
{
	OnXPAmountChangedDelegate.Broadcast(TotalExperienceAmount);
}
