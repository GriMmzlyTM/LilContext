// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "Context_Holder.generated.h"

class UContext_Action;
class UContext_ActionPayloadBase;
class UContext_ActionEntry;

UINTERFACE(BlueprintType, Blueprintable)
class CONTEXT_API UContext_Holder : public UInterface {
	GENERATED_BODY()
};

/**
 * Signifies that an entity holds a context and is a contextual object.
 * This is the interface required if you want your object to be able to open a context menu, and contain context entries
 *
 * This entity will receive context entries from its parents that implement the Giver interface
 */
class CONTEXT_API IContext_Holder {
	GENERATED_BODY()

public:
	/**
	 * Gets the tags the entity has, which is useful for determining if the holder can execute an action 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Holder|Info")
	FGameplayTagContainer GetTags() const;
	
	/**
	 * Get the display name for this entity
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Holder|Info")
	FText GetDisplayName() const;
	
	/**
	 * Get the current position of this entity in the world
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Holder|Transform")
	FVector GetPosition() const;
	
	/**
	 * Get all actions listed on the context holder
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Holder|Action")
	TSet<UContext_ActionEntry*> GetActionEntries() const;

	/**
	 * Gets a priority ordered list of primary action entries. Primary action entries being the entries that should
	 * be executed by default when interacting with an object.
	 * For example, equipping an inventory item, picking up an item off the ground, opening a door, etc.
	 * @return Context entry if valid, or null
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Holder|Action")
	TArray<UContext_ActionEntry*> GetPrimaryActionEntries() const;
	
	/**
	 * Get the payload that was provided for the specific action. Payload may be null.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Holder|Action", meta=(DeprecatedFunction))
	const UContext_ActionPayloadBase* RequestPayloadOfType(TSubclassOf<UContext_ActionPayloadBase> PayloadType) const;

	/**
	 * Get the payload that was provided for the specific action. Payload may be null.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Holder|Action")
	const UContext_ActionPayloadBase* RequestPayload(const UContext_ActionEntry* ActionEntry) const;
	
};
