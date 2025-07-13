


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Owner);

	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if (CombatInterface)
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(
			this,
			[this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved)
				.AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
		});
	}

	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (IsValid(GetOwner()) && GetOwner()->Implements<UCombatInterface>())
	{
		if (ICombatInterface::Execute_IsDead(GetOwner()))
		{
			Deactivate();
			return;
		}
	}
	
	if (NewCount > 0) Activate();
	else Deactivate();	
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* Owner)
{
	Deactivate();
}
