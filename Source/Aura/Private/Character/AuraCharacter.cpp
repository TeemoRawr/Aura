


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400.f, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	TObjectPtr<AAuraPlayerState> AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	
	UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent());
	AuraAbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
	AuraAbilitySystemComponent->AbilityActorInfoSet();
	
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(
				AuraPlayerController,
				AuraPlayerState,
				AbilitySystemComponent,
				AttributeSet);
		}
	}

	InitializeDefaultAttributes();
}

int32 AAuraCharacter::GetPlayerLevel()
{
	const TObjectPtr<AAuraPlayerState> AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerLevel();
}
