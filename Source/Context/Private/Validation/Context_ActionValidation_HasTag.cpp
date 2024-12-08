// Fill out your copyright notice in the Description page of Project Settings.


#include "Validation/Context_ActionValidation_HasTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

bool UContext_ActionValidation_HasTag::OnValidationStart_Implementation(AActor* Entity) {
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Entity);
	if (IsValid(ASC)) {
		return ASC->HasAllMatchingGameplayTags(RequiredTags);
	}

	if (Entity->Implements<UGameplayTagAssetInterface>()) {
		const IGameplayTagAssetInterface* EntityGameplayTagInterface = Cast<IGameplayTagAssetInterface>(Entity);
		return EntityGameplayTagInterface->HasAllMatchingGameplayTags(RequiredTags);
	}

	UE_LOG(LogContextValidation, Warning, TEXT("Unable to find valid gameplay tags on %s"), *Entity->GetName());
	return false;
}
