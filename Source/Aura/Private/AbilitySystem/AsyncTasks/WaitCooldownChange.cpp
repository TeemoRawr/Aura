


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(
	UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->AbilitySystemComponent = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(WaitCooldownChange) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag)
		.AddUObject(WaitCooldownChange, &UWaitCooldownChange::CooldownTagChanged);

	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf
		.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);
	
	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(AbilitySystemComponent)) return;
	AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(
	UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied,
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer AssetsTags;
	SpecApplied.GetAllAssetTags(AssetsTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	if (AssetsTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemaining(GameplayEffectQuery);

		if (TimesRemaining.Num() > 0)
		{
			const float TimeRemaining = FMath::Max(TimesRemaining);
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}
