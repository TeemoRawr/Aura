


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if (!GetAuraAbilitySystemComponent()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		const FGameplayTag AbilityTagFromSpec = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec);
		
		FAuraAbilityInfo AuraAbilityInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTagFromSpec);
		
		AuraAbilityInfo.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		AuraAbilityInfo.StatusTag = UAuraAbilitySystemComponent::GetStatusFromSpec(AbilitySpec);
		
		AbilityInfoDelegate.Broadcast(AuraAbilityInfo);
	});

	GetAuraAbilitySystemComponent()->ForEachAbility(BroadcastDelegate);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPlayerController()
{
	if (AuraPlayerController == nullptr)
	{
		AuraPlayerController = CastChecked<AAuraPlayerController>(PlayerController);
	}

	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPlayerState()
{
	if (AuraPlayerState == nullptr)
	{
		AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	}

	return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraAbilitySystemComponent()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}

	return AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAttributeSet()
{
	if (AuraAttributeSet == nullptr)
	{
		AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	}

	return AuraAttributeSet;
}
