// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Context_EntryButton.generated.h"

class UContext_Action;
class IContext_Holder;
class UContext_ActionEntry;
/**
 * A usable button that serves as the UI representation of a context entry, linked to an entity.
 */
UCLASS()
class CONTEXT_API UContext_EntryButton : public UButton {
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	TWeakObjectPtr<UTextBlock> ContextActionName;

	UPROPERTY(meta=(BindWidget))
	TWeakObjectPtr<UTextBlock> ContextEntityName;

	UPROPERTY()
	const UContext_ActionEntry* ContextAction;

	UPROPERTY()
	AActor* InstigatingActor;

	UPROPERTY()
	TScriptInterface<IContext_Holder> ContextObject;
	
public:
	UFUNCTION(BlueprintCallable)
	void Setup(AActor* Instigator, const UContext_ActionEntry* ContextEntry, const TScriptInterface<IContext_Holder> ContextHolder); 

private:

	UFUNCTION()
	void ActionSelected();
};
