// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Context_ActionValidationResult.h"
#include "Context_ActionValidationResult_ShowWorldDisplayMessage.generated.h"

/**
 * 
 */
UCLASS()
class CONTEXT_API UContext_ActionValidationResult_ShowWorldDisplayMessage : public UContext_ActionValidationResult {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText DisplayText;
	
public:
	virtual void OnResultActionStart_Implementation(AActor* Entity) override;
};
