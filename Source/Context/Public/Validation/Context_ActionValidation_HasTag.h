// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Context_ActionValidation.h"
#include "GameplayTagContainer.h"
#include "Context_ActionValidation_HasTag.generated.h"

/**
 * 
 */
UCLASS()
class CONTEXT_API UContext_ActionValidation_HasTag : public UContext_ActionValidation {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredTags;
	
protected:
	virtual bool OnValidationStart_Implementation(AActor* Entity) override;
};
