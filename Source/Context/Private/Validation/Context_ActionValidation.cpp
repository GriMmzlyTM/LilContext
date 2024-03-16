// Fill out your copyright notice in the Description page of Project Settings.


#include "Validation/Context_ActionValidation.h"

#include "Validation/Result/Context_ActionValidationResult.h"

bool UContext_ActionValidation::RunValidation_Implementation(AActor* Caller, AActor* ContextOwner) {
	bool bSuccess = false;
	switch (ValidationSubject) {
		case EActionValidation_Subject::Caller:
			bSuccess = OnValidationStart(Caller);
			break;
		case EActionValidation_Subject::ContextOwner:
			bSuccess = OnValidationStart(ContextOwner);
			break;
		case Both:
			bSuccess = OnValidationStart(Caller) && OnValidationStart(ContextOwner);
			break;
	}
	
	// Execute desired result actions (Show errors to player, give items, whatever)
	TArray<UContext_ActionValidationResult*> ResultToExecute = bSuccess ? OnSuccess : OnFail;
	for (const auto ResultAction : ResultToExecute) {
		ResultAction->RunResultAction(Caller, ContextOwner);
	}

	// If we allow failure, then the validation will always return true, but the result will still be executed
	if (ValidationSeverity == AllowFail) return true;
	return bSuccess;
	
}

bool UContext_ActionValidation::OnValidationStart_Implementation(AActor* Entity) {
	return true;
}
