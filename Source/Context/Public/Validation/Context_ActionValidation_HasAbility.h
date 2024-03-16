// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Context_ActionValidation.h"
#include "GameplayTagContainer.h"
#include "Context_ActionValidation_HasAbility.generated.h"

/**
 * 
 */
UCLASS()
class CONTEXT_API UContext_ActionValidation_HasAbility : public UContext_ActionValidation {
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredAbilityTag;

protected:
	virtual bool OnValidationStart_Implementation(AActor* Entity) override;

	
};
