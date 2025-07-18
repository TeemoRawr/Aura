


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
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

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->IsSuccessfulDebuff();	
	}

	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->GetDebuffDamage();	
	}

	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->GetDebuffDuration();	
	}

	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->GetDebuffFrequency();	
	}

	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		if (AuraGameplayEffectContext->GetDamageType().IsValid())
		{
			return *AuraGameplayEffectContext->GetDamageType();	
		}
	}

	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->GetDeathImpulse();
	}

	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		return AuraGameplayEffectContext->GetKnockbackForce();
	}

	return FVector::ZeroVector;
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

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,bool bIsSuccessfulDebuff)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetIsSuccessfulDebuff(bIsSuccessfulDebuff);	
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetDebuffDamage(InDebuffDamage);	
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{		
		AuraGameplayEffectContext->SetDamageType(MakeShared<FGameplayTag>(InDamageType));	
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDuration)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetDebuffDuration(InDebuffDuration);	
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffFrequency)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetDebuffFrequency(InDebuffFrequency);	
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,const FVector& InImpulse)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetDeathImpulse(InImpulse);	
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,	const FVector& InImpulse)
{
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());

	if (AuraGameplayEffectContext)
	{
		AuraGameplayEffectContext->SetKnockbackForce(InImpulse);	
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

void UAuraAbilitySystemLibrary::GetClosestTargets(
	const int32 MaxTargets,
	const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets,
	const FVector& Origin)
{
	if (MaxTargets >= Actors.Num())
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;

	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.IsEmpty()) break;
		
		double ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor = nullptr;
		
		for (AActor* PotentialTarget : ActorsToCheck)
		{
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}

		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		NumTargetsFound++;
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

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);

	const bool bIsKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;

	if (bIsKnockback)
	{
		SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);
	}

	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
		DamageEffectParams.DamageGameplayEffectClass,
		DamageEffectParams.AbilityLevel,
		EffectContextHandle);

	// Debuff
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	return EffectContextHandle;
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

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;
	
	if (NumRotators > 1)
	{
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2, Axis);		
		const float DeltaSpread = Spread / (NumRotators - 1);
		
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}

	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotaredVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;
	
	if (NumVectors > 1)
	{
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2, Axis);		
		const float DeltaSpread = Spread / (NumVectors - 1);
		
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}

	return Vectors;
}
