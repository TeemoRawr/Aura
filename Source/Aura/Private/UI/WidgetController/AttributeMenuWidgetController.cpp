


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (auto& Pair : AuraAttributeSet->TagsToAttributes)
	{
		FGameplayAttribute GameplayAttribute = Pair.Value();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GameplayAttribute).AddLambda(
			[this, Pair, AuraAttributeSet, GameplayAttribute](const FOnAttributeChangeData& Data)
			{
				FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
				Info.AttributeValue = GameplayAttribute.GetNumericValue(AuraAttributeSet);

				AttributeInfoDelegate.Broadcast(Info);
			}
		);
	}

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnAttributePointsChangedDelegate.AddLambda([this](const int32 NewValue)
	{
		OnAttributePointsChangedDelegate.Broadcast(NewValue);
	});
	AuraPlayerState->OnSpellPointsChangedDelegate.AddLambda([this](const int32 NewValue)
	{
		OnSpellPointsChangedDelegate.Broadcast(NewValue);
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (auto& Pair : AuraAttributeSet->TagsToAttributes)
	{
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
		FGameplayAttribute GameplayAttribute = Pair.Value();
		Info.AttributeValue = GameplayAttribute.GetNumericValue(AuraAttributeSet);

		AttributeInfoDelegate.Broadcast(Info);
	}

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnAttributePointsChangedDelegate.Broadcast(AuraPlayerState->GetAttributePoints());
	OnSpellPointsChangedDelegate.Broadcast(AuraPlayerState->GetSpellPoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraAbilitySystemComponent->UpgradeAttribute(AttributeTag);
}
