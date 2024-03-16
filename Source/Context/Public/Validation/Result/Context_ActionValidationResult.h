// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Validation/Context_ActionValidation.h"
#include "Context_ActionValidationResult.generated.h"

/**
 * Action to take when a validation has succeeded, or failed. 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class CONTEXT_API UContext_ActionValidationResult : public UObject {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EActionValidation_Subject> ValidationSubject;
	
public:
	/// Function called by externals to trigger the action result
	/// @param Caller 
	/// @param ContextOwner 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Context|Validation|Result")
	void RunResultAction(AActor* Caller, AActor* ContextOwner);

protected:
	/// Internal function that actually has functionality. This is what you need to override!
	/// @param Entity 
	UFUNCTION(BlueprintNativeEvent, Category="Context|Validation|Result")
	void OnResultActionStart(AActor* Entity);
};
