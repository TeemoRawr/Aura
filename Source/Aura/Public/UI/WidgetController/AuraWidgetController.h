

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

class UAbilityInfo;
class UAuraAttributeSet;
class UAuraAbilitySystemComponent;
class AAuraPlayerState;
class AAuraPlayerController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);

class UAttributeSet;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}
	FWidgetControllerParams(
		APlayerController* PlayerController,
		APlayerState* PlayerState,
		UAbilitySystemComponent* AbilitySystemComponent,
		UAttributeSet* AttributeSet)
	: PlayerController(PlayerController), PlayerState(PlayerState), AbilitySystemComponent(AbilitySystemComponent), AttributeSet(AttributeSet)  {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

/**
 * 
 */
UCLASS(Abstract)
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;
	
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();

	void BroadcastAbilityInfo();

protected:
	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UFUNCTION(BlueprintCallable, Category="Widget Controller")
	AAuraPlayerController* GetAuraPlayerController();

	UFUNCTION(BlueprintCallable, Category="Widget Controller")
	AAuraPlayerState* GetAuraPlayerState();

	UFUNCTION(BlueprintCallable, Category="Widget Controller")
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	UFUNCTION(BlueprintCallable, Category="Widget Controller")
	UAuraAttributeSet* GetAuraAttributeSet();
	
private:
	UPROPERTY()
	TObjectPtr<AAuraPlayerController> AuraPlayerController;

	UPROPERTY()
	TObjectPtr<AAuraPlayerState> AuraPlayerState;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AuraAttributeSet;
};
