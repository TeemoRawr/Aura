#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

// #if UE_WITH_IRIS
// #include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
// #include "Serialization/GameplayEffectContextNetSerializer.h"
// #endif

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsCriticalHit() const { return bIsBlockedHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }

	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsBlockedHit = bInIsCriticalHit; }
	void SetIsBlockedHit(const bool bInIsBlockerHit) { bIsBlockedHit = bInIsBlockerHit; }
	
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	} 
	
protected:
	UPROPERTY()
	bool bIsBlockedHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true,
	};
};

// #if UE_WITH_IRIS
// namespace UE::Net
// {
// 	// Forward to FGameplayEffectContextNetSerializer
// 	// Note: If FLyraGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
// 	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(AuraGameplayEffectContext, FGameplayEffectContextNetSerializer);
// }
// #endif