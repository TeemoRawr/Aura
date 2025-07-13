


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	if (Level == 1)
	{
		return FString::Printf(TEXT(
				"<Title>FIRE BOLT</>\n\n"
				"<Default>Lunches a bolt of ire exploding on impact and dealing </><Damage>%i</><Default> fire damage with a chance to burn</>\n\n"
				"<Small>Level: </><Level>%d</>\n"
				"<Small>Mana cost: </><ManaCost>%.1f</>\n"
				"<Small>Cooldown: </><Cooldown>%.1f</>\n"),
			static_cast<int>(ScaledDamage), Level, ManaCost, Cooldown);
	}

	return FString::Printf(TEXT(
		"<Title>FIRE BOLT</>\n\n"
		"<Default>Lunches %d bolts of ire exploding on impact and dealing </><Damage>%d</><Default> fire damage with a chance to burn</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"),
		FMath::Min(Level, NumberOfProjectiles), static_cast<int>(ScaledDamage), Level, ManaCost, Cooldown);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		"<Title>FIRE BOLT</>\n\n"
		"<Default>Lunches %d bolts of ire exploding on impact and dealing </><Damage>%i</><Default> fire damage with a chance to burn</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>"),
		FMath::Min(Level, NumberOfProjectiles), static_cast<int>(ScaledDamage), Level, ManaCost, Cooldown);
}

void UAuraFireBolt::SpawnProjectiles(
	const FVector& ProjectileTargetLocation,
	const FGameplayTag& SocketTag,
	bool bOverridePitch,
	float PitchOverride,
	AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();

	if (!bIsServer) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(),	SocketTag);
	
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	const FVector Forward = Rotation.Vector();

	const int32 NumProjectiles = FMath::Min(MaxNumberOfProjectiles, GetAbilityLevel());

	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, NumProjectiles);

	for (const FRotator& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());
	
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageParamsFromClassDefaults();

		if (IsValid(HomingTarget) && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}

		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bIsLunchHomingProjectiles;

		Projectile->FinishSpawning(SpawnTransform);
	}
}
