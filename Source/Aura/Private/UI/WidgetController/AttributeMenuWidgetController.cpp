


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

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
}
