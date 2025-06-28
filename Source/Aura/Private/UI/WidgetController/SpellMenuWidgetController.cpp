


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	OnSpellPointsChangedDelegate.Broadcast(GetAuraPlayerState()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraAbilitySystemComponent()->AbilityStatusChanged
		.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
		{
			if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
			{
				SelectedAbility.Status = StatusTag;

				bool bEnableSpellPointsButtons = false;
				bool bEnableEquipButtons = false;

				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpellPointsButtons, bEnableEquipButtons);

				FString Description;
				FString NextLevelDescription;

				GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
				
				SpellGlobeSelectedDelegate.Broadcast(
					bEnableSpellPointsButtons,
					bEnableEquipButtons,
					Description,
					NextLevelDescription);
			}
			
			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;

				AbilityInfoDelegate.Broadcast(Info);
			}
		});

	GetAuraPlayerState()->OnSpellPointsChangedDelegate.AddLambda([this](const int32 SpellPoints)
	{
		OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
		CurrentSpellPoints = SpellPoints;

		bool bEnableSpellPointsButtons = false;
		bool bEnableEquipButtons = false;

		ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpellPointsButtons, bEnableEquipButtons);

		FString Description;
		FString NextLevelDescription;

		GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		
		SpellGlobeSelectedDelegate.Broadcast(
			bEnableSpellPointsButtons,
			bEnableEquipButtons,
			Description,
			NextLevelDescription);
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const int32 SpellPoints = GetAuraPlayerState()->GetSpellPoints();
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	
	FGameplayTag AbilityStatus;

	FGameplayAbilitySpec* AbilitySpec = GetAuraAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag);
	
	const bool bTagIsValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const bool bSpecValid = AbilitySpec != nullptr; 

	if (!bTagIsValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraAbilitySystemComponent()->GetStatusFromSpec(*AbilitySpec);
	}

	SelectedAbility = {  AbilityTag, AbilityStatus };

	bool bEnableSpellPointsButtons = false;
	bool bEnableEquipButtons = false;

	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpellPointsButtons, bEnableEquipButtons);

	FString Description;
	FString NextLevelDescription;

	GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(
		bEnableSpellPointsButtons,
		bEnableEquipButtons,
		Description,
		NextLevelDescription);
}

void USpellMenuWidgetController::SpendPointsButtonPressed()
{
	GetAuraAbilitySystemComponent()->ServerSpendPoint(SelectedAbility.Ability);
}

void USpellMenuWidgetController::ShouldEnableButtons(
	const FGameplayTag& AbilityStatus,
	const int32 SpellPoints,
	bool& bShouldEnableSpellPointsButtons,
	bool& bShouldEnableEquipButtons)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButtons = true;
		bShouldEnableSpellPointsButtons = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		bShouldEnableEquipButtons = false;
		bShouldEnableSpellPointsButtons = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButtons = true;
		bShouldEnableSpellPointsButtons = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
	{
		bShouldEnableEquipButtons = false;
		bShouldEnableSpellPointsButtons = false;
	}

	if (SpellPoints == 0)
	{
		bShouldEnableSpellPointsButtons = false;
	}
}
