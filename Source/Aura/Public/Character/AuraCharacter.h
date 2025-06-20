

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Player Interface **/
	virtual void AddToXP_Implementation(const int32& InXP) override;
	virtual void AddToPlayerLevel_Implementation(const int32& InLevel) override;
	virtual void AddToAttributePoints_Implementation(const int32& InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(const int32& InSpellPoints) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() override;
	virtual int32 GetAttributePointsReward_Implementation(const int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(const int32 Level) const override;
	virtual int32 FindLevelForXP_Implementation(const int32 InXP) const override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;

	/** Combat Interface **/
	virtual int32 GetPlayerLevel_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	virtual void InitAbilityActorInfo() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticle() const;
};
