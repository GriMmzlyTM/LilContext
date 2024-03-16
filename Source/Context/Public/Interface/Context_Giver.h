// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Context_Giver.generated.h"

class UContext_ActionPayloadBase;
class UContext_ActionEntry;

UINTERFACE(BlueprintType, Blueprintable)
class CONTEXT_API UContext_Giver : public UInterface {
	GENERATED_BODY()
};

/**
 * A context giver interface signifies that this entity should pass a context down to its children
 * This allows context objects to understand their hierarchy.
 */
class CONTEXT_API IContext_Giver {
	GENERATED_BODY()
	
public:
	/**
	 * Adds a context entry to the giver, which will pass it down to its context holder children
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Giver|Entries")
	void AddContextEntry(UContext_ActionEntry* ContextEntry);

	/**
	 * Removes a context entry from the giver. This will not remove contexts from its children.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Giver|Entries")
	void RemoveContextEntry(UContext_ActionEntry* ContextEntry);

	/**
	 * Get all contexts this giver contains and will pass to its children
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Giver|Entries")
	void GetGiverContextEntries(TSet<UContext_ActionEntry*>& OutContextEntries);

	/**
	 * Gets the primary context passed down, if available.
	 * The context found will be used. If context is not applying, it may be overridden lower down in the tree
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Giver|Entries")
	UContext_ActionEntry* GetPrimaryContextEntry();
	
	/**
	 * Requests a payload from the context giver if a payload is setup.
	 * @param PayloadClass The class time to search or
	 * @return Payload if valid, or null
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Context|Giver|Payload")
	UContext_ActionPayloadBase* RequestContextPayload(TSubclassOf<UContext_ActionPayloadBase> PayloadClass);
};
