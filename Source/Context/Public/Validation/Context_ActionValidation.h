// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Context_ActionValidation.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogContextValidation, Log, All);

class UContext_ActionValidationResult;

UENUM(BlueprintType)
enum EActionValidation_Subject {
	Caller,
	ContextOwner,
	Both
};

UENUM(BlueprintType)
enum EActionValidation_Severity {
	AllowFail,
	NoFail
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class CONTEXT_API UContext_ActionValidation : public UObject {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EActionValidation_Subject> ValidationSubject;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EActionValidation_Severity> ValidationSeverity;

	UPROPERTY(EditDefaultsOnly)
	TArray<UContext_ActionValidationResult*> OnSuccess;

	UPROPERTY(EditDefaultsOnly)
	TArray<UContext_ActionValidationResult*> OnFail;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Context|Validation")
	bool RunValidation(AActor* Caller, AActor* ContextOwner); 
	
protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Context|Validation")
	bool OnValidationStart(AActor* Entity); 
};
