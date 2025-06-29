


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

	GetAuraAbilitySystemComponent()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag& AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipSelectionDelegate.Broadcast(AbilityType);
		bWaitingForEquipSelection = false;
	}
	
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

void USpellMenuWidgetController::GlobeDeselected()
{
	if (bWaitingForEquipSelection)
	{
		const FGameplayTag& AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitingForEquipSelectionDelegate.Broadcast(AbilityType);
		bWaitingForEquipSelection = false;
	}
	
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	
	SelectedAbility = { GameplayTags.Abilities_None, GameplayTags.Abilities_Status_Locked };

	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag& AbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;

	WaitForEquipSelectionDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection = true;
	const FGameplayTag& SelectedStatus = GetAuraAbilitySystemComponent()->GetStatusFromAbilityTag(SelectedAbility.Ability);

	if (SelectedStatus.MatchesTag(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraAbilitySystemComponent()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if (!bWaitingForEquipSelection) return;

	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;

	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetAuraAbilitySystemComponent()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
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

void USpellMenuWidgetController::OnAbilityEquipped(
	const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag,
	const FGameplayTag& SlotTag,
	const FGameplayTag& PrevSlotTag)
{
	bWaitingForEquipSelection = false;

	FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PrevSlotTag;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;

	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = SlotTag;

	AbilityInfoDelegate.Broadcast(Info);

	StopWaitingForEquipSelectionDelegate.Broadcast(Info.AbilityType);
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselected();
}
