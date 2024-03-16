// Fill out your copyright notice in the Description page of Project Settings.


#include "Validation/Context_ActionValidation_HasAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

bool UContext_ActionValidation_HasAbility::OnValidationStart_Implementation(AActor* Entity) {
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Entity);
	if (!IsValid(ASC)) return false;

	// Find how many (If any) of the ability we have.
	TArray<FGameplayAbilitySpecHandle> OutAbilityHandles;
	ASC->FindAllAbilitiesWithTags(OutAbilityHandles, RequiredAbilityTag);
	return OutAbilityHandles.Num() != 0;
}
