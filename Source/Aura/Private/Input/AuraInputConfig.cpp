


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(
	const FGameplayTag& InputTag,
	const bool bLogNotFound) const
{
	for (const FAuraInputAction InputAction : AbilityInputActions)
	{
		if (InputAction.InputAction && InputAction.InputTag == InputTag)
		{
			return  InputAction.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("InputAction not found for ability %s"), *InputTag.ToString());
	}

	return nullptr;
}
