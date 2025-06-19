


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
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::SetXP(int32 NewTotalExperienceAmount)
{
	TotalExperienceAmount = NewTotalExperienceAmount;
	OnTotalExperienceAmountChanged.Broadcast(TotalExperienceAmount);
}

void AAuraPlayerState::SetLevel(int32 NewLevel)
{
	Level = NewLevel;
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::AddToLevel(int32 LevelsToAdd)
{
	Level += LevelsToAdd;
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::AddXP(int32 ExperienceAmountToAdd)
{
	TotalExperienceAmount += ExperienceAmountToAdd;
	OnTotalExperienceAmountChanged.Broadcast(TotalExperienceAmount);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChanged.Broadcast(Level);
}

void AAuraPlayerState::OnRep_TotalExperienceAmount(int32 OldTotalExperienceAmount)
{
	OnTotalExperienceAmountChanged.Broadcast(TotalExperienceAmount);
}
