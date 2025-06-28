


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "Engine/OverlapResult.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& WidgetParamsOut)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
		UAbilitySystemComponent* AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
		UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();

		WidgetParamsOut = FWidgetControllerParams(
			PlayerController,
			PlayerState,
			AbilitySystemComponent,
			AttributeSet);

		return true;
	}

	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD()))
		{
			FWidgetControllerParams WidgetControllerParams;

			if (!MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams))
			{
				return nullptr;
			}
			
			UOverlayWidgetController* OverlayWidgetController = AuraHUD->GetOverlayWidgetController(WidgetControllerParams);

			return OverlayWidgetController;
		}
	}

	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD()))
		{
			FWidgetControllerParams WidgetControllerParams;

			if (!MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams))
			{
				return nullptr;
			}
			
			UAttributeMenuWidgetController* AttributeMenuWidgetController = AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);

			return AttributeMenuWidgetController;
		}
	}

	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD()))
		{
			FWidgetControllerParams WidgetControllerParams;

			if (!MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams))
			{
				return nullptr;
			}
			
			USpellMenuWidgetController* SpellMenuWidgetController = AuraHUD->GetSpellMenuWidgetController(WidgetControllerParams);

			return SpellMenuWidgetController;
		}
	}

	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(
	const UObject* WorldContextObject,
	const ECharacterClass CharacterClass,
	const float Level,
	UAbilitySystemComponent* AbilitySystemComponent)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	const AActor* AvatarActor = AbilitySystemComponent->GetAvatarActor();	
	
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = AbilitySystemComponent->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	
	const FGameplayEffectSpecHandle& PrimaryAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		ClassDefaultInfo.PrimaryAttribute,
		Level,
		PrimaryAttributesContextHandle);

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = AbilitySystemComponent->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	
	const FGameplayEffectSpecHandle& SecondaryAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		CharacterClassInfo->SecondaryAttribute,
		Level,
		SecondaryAttributesContextHandle);

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = AbilitySystemComponent->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);	
	const FGameplayEffectSpecHandle& VitalAttributesSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		CharacterClassInfo->VitalAttribute,
		Level,
		VitalAttributesContextHandle);
	
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject,	UAbilitySystemComponent* AbilitySystemComponent, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	if (CharacterClassInfo == nullptr) return;
	
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}

	const FCharacterClassDefaultInfo& CharacterClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	const bool bImplementsCombatInterface = AbilitySystemComponent->GetAvatarActor()->Implements<UCombatInterface>();
	
	const int32 PlayerLevel = bImplementsCombatInterface
		? ICombatInterface::Execute_GetPlayerLevel(AbilitySystemComponent->GetAvatarActor())
		: 1;
	
	for (TSubclassOf<UGameplayAbility> StartupAbility : CharacterClassDefaultInfo.StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(StartupAbility, PlayerLevel);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));

	if (AuraGameMode == nullptr) return nullptr;
	
	return  AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));

	if (AuraGameMode == nullptr) return nullptr;
	
	return  AuraGameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->IsBlockedHit();	
	}

	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->IsCriticalHit();	
	}

	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsBlockedHit)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetIsBlockedHit(bIsBlockedHit);	
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsCriticalHit)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetIsCriticalHit(bIsCriticalHit);	
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
	const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors,
	const TArray<AActor*> ActorsToIgnore,
	float Radius,
	const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> OverlapResults;
		World->OverlapMultiByObjectType(
			OverlapResults,
			SphereOrigin,
			FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius),
			SphereParams);

		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			AActor* OverlappingActor = OverlapResult.GetActor();
			bool bHasCombatInterface = OverlappingActor->Implements<UCombatInterface>();

			if (!bHasCombatInterface) continue;
			const bool bIsDead = ICombatInterface::Execute_IsDead(OverlappingActor);
			if (!bIsDead)
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(OverlappingActor));
			}
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bIsFirstActorPlayer = FirstActor->ActorHasTag(FName("Player"));
	const bool bIsFSecondActorPlayer = SecondActor->ActorHasTag(FName("Player"));

	if (bIsFirstActorPlayer && bIsFSecondActorPlayer)
	{
		return false;
	}
	
	const bool bIsFirstActorEnemy = FirstActor->ActorHasTag(FName("Enemy"));
	const bool bIsFSecondActorEnemy = SecondActor->ActorHasTag(FName("Enemy"));

	if (bIsFirstActorEnemy && bIsFSecondActorEnemy)
	{
		return false;
	}

	return true;
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForCharacterClassAndLevel(
	const UObject* WorldContextObject,
	ECharacterClass CharacterClass,
	int32 Level)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	if (CharacterClassInfo == nullptr) return 0;

	const FCharacterClassDefaultInfo& CharacterClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = CharacterClassDefaultInfo.XPReward.GetValueAtLevel(Level);

	return static_cast<int32>(XPReward);
}
