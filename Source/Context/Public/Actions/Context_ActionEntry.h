// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Context_ActionEntry.generated.h"

class UContext_ActionValidation;
class UContext_Action;
/**
 * An action entry is a container for an action used anywhere that it needs to be represented.
 * Actions do not have names, descriptions, or anything else that can be used by UI or example
 * Actions are also agnostic and don't care about their names.
 * This allows you to have multiple entries that use the same action with different settings and names.
 *
 * This is what most systems interact with and use to get the action.
 */
UCLASS(BlueprintType)
class CONTEXT_API UContext_ActionEntry : public UDataAsset {
	GENERATED_BODY()
	
public:
	
	/**
	 * The display name of the entry (Open, Equip, Examine, etc)
	 */
	UPROPERTY(EditDefaultsOnly)
	FText ActionName;

	/**
	 * The description of the entry
	 * This is used for tooltips
	 */
	UPROPERTY(EditDefaultsOnly)
	FText ActionDescription;

	/**
	 * If true, will attempt to display the context item's name next to the action entry name
	 */
	UPROPERTY(EditDefaultsOnly)
	bool bDisplayEntityName = true;
	
	/**
	* Tags that are required for this action to be executable. All are required.
	* Requires exact match.
	* if Tag.A.B is required, Tag.A.B.C will not be valid, nor will Tag.A
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer RequiredTags;

	/**
	* Tags that if present, block the action. Any will block.
	* Blocks general match. Tag.A will block Tag.A Tag.A.B, etc.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockingTags;

	UPROPERTY(EditDefaultsOnly, Category = "Validation")
	TArray<UContext_ActionValidation*> Validations;

	/**
	 * The action that should be executed when the menu entry is chosen
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UContext_Action> Action;
	
	/// Runs validations on the context action, and returns it if it's valid. 
	/// @param Caller 
	/// @param ContextOwner 
	/// @return 
	UFUNCTION(BlueprintCallable, Category="Context")
	bool RunActionValidations(AActor* Caller, AActor* ContextOwner) const;
};
