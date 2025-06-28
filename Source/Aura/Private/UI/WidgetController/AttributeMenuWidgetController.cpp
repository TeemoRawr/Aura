


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	for (auto& Pair : GetAuraAttributeSet()->TagsToAttributes)
	{
		FGameplayAttribute GameplayAttribute = Pair.Value();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GameplayAttribute).AddLambda(
			[this, Pair, GameplayAttribute](const FOnAttributeChangeData& Data)
			{
				FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
				Info.AttributeValue = GameplayAttribute.GetNumericValue(GetAuraAttributeSet());

				AttributeInfoDelegate.Broadcast(Info);
			}
		);
	}

	GetAuraPlayerState()->OnAttributePointsChangedDelegate.AddLambda([this](const int32 NewValue)
	{
		OnAttributePointsChangedDelegate.Broadcast(NewValue);
	});
	GetAuraPlayerState()->OnSpellPointsChangedDelegate.AddLambda([this](const int32 NewValue)
	{
		OnSpellPointsChangedDelegate.Broadcast(NewValue);
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	for (auto& Pair : GetAuraAttributeSet()->TagsToAttributes)
	{
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
		FGameplayAttribute GameplayAttribute = Pair.Value();
		Info.AttributeValue = GameplayAttribute.GetNumericValue(GetAuraAttributeSet());

		AttributeInfoDelegate.Broadcast(Info);
	}

	OnAttributePointsChangedDelegate.Broadcast(GetAuraPlayerState()->GetAttributePoints());
	OnSpellPointsChangedDelegate.Broadcast(GetAuraPlayerState()->GetSpellPoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraAbilitySystemComponent()->UpgradeAttribute(AttributeTag);
}
