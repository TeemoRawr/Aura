


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}


	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	check(EnhancedInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);	
}

void AAuraPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	const FVector2d InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControllerPawn = GetPawn<APawn>())
	{
		ControllerPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControllerPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CurrentHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CurrentHit);

	if (!CurrentHit.bBlockingHit) return;
	LastActor = ThisActor;
	ThisActor = CurrentHit.GetActor();

	/**
	 * Line trace from cursor. There are several scenarios
	 * A. LastActor is null && ThisActor is null
	 *   - Do nothing
	 * B Last is null && ThisActor is valid
	 *   - Call Hightlight ThisActor
	 * C LastActor is valid && ThisActor is null
	 *   - Call UnHightlight LastActor
	 * D LastActor is valid && ThisActor is valid but LastActor != ThisActor
	 *   - Call UnHightlight LastActor
	 *   - Call Hightlight ThisActor
	 * E LastActor is valid && ThisActor is valid but LastActor == ThisActor
	 *   - Do nothing
	 */

	if (LastActor == nullptr)
	{
		// Case B
		if (ThisActor != nullptr)
		{
			ThisActor->HighlightActor();
		}
		else
		{
			// Case A
		}
	}
	else
	{
		// Case C
		if (ThisActor == nullptr)
		{
			LastActor->UnHighlightActor();
		}
		else
		{
			// Case D
			if (LastActor != ThisActor)
			{
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// Case E
			}
		}
	}
}
