// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Validation/Result/Context_ActionValidationResult.h"
#include "Context_ActionValidationResult_PlaySound.generated.h"

/**
 * 
 */
UCLASS()
class CONTEXT_API UContext_ActionValidationResult_PlaySound : public UContext_ActionValidationResult {
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> SoundCueToPlay;

protected:
	virtual void OnResultActionStart_Implementation(AActor* Entity) override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
