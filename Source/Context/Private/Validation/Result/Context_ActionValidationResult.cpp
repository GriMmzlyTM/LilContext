// Fill out your copyright notice in the Description page of Project Settings.


#include "Validation/Result/Context_ActionValidationResult.h"

void UContext_ActionValidationResult::RunResultAction_Implementation(AActor* Caller, AActor* ContextOwner) {
	switch (ValidationSubject) {
		case EActionValidation_Subject::Caller:
			OnResultActionStart(Caller);
			break;
		case EActionValidation_Subject::ContextOwner:
			OnResultActionStart(ContextOwner);
			break;
		case Both:
			OnResultActionStart(Caller);
			OnResultActionStart(ContextOwner);
			break;
	}
}

void UContext_ActionValidationResult::OnResultActionStart_Implementation(AActor* Entity) {
}
