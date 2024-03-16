// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Context_ActionEntry.h"

#include "Validation/Context_ActionValidation.h"


bool UContext_ActionEntry::RunActionValidations(AActor* Caller, AActor* ContextOwner) const {

	for (const auto Validation : Validations) {
		if (!Validation->RunValidation(Caller, ContextOwner)) {
			return false;
		}
	}

	return true;
}
