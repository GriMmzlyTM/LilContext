// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonBorder.h"
#include "Blueprint/UserWidget.h"
#include "Interface/Context_Holder.h"
#include "Context_UIWidgetBase.generated.h"

/**
 * Base class for widgets that should be contextual and allow opening a menu
 *
 * As a context holder, it receives context from its Context Giver parents.
 */
UCLASS()
class CONTEXT_API UContext_UIWidgetBase : public UUserWidget, public IContext_Holder {
	GENERATED_BODY()
	
	/**
	 * These are the tags owned by this UI. Unlike for an AActor, these are the only tags used for UI
	 * The UI element does NOT have an ability system implemented, so you must manually add/remove tags from this UI
	 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "Context|UI")
	FGameplayTagContainer DefaultTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Context|UI")
	TSet<UContext_ActionEntry*> ContextEntries;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Context|UI")
	TArray<UContext_ActionEntry*> PrimaryContextEntryPriority;
	
private:
	
	// Return handled to prevent calling OnMouseLeave during click
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
public:
	
	/**
	 * Grants a tag to the UI element, which will enable or disable certain contexts
	 */
	UFUNCTION(BlueprintCallable, Category="Context|UI|Tags")
	void GiveTag(FGameplayTagContainer Tags);

	/// Remove a tag from the UI element, which will enable or disable certain contexts
	UFUNCTION(BlueprintCallable, Category="Context|UI|Tags")
	void RemoveTag(FGameplayTagContainer Tags);

	UFUNCTION(BlueprintCallable, Category="Context|UI|Tags")
	void ClearTags();
	
	// ~IContext_Holder Implementation
	virtual FGameplayTagContainer GetTags_Implementation() const override { return DefaultTags; }
	
	virtual FVector GetPosition_Implementation() const override;
	virtual TSet<UContext_ActionEntry*> GetActionEntries_Implementation() const override;
	virtual const UContext_ActionPayloadBase* RequestPayloadOfType_Implementation(TSubclassOf<UContext_ActionPayloadBase> PayloadType) const override;
	virtual TArray<UContext_ActionEntry*> GetPrimaryActionEntries_Implementation() const override;
	virtual const UContext_ActionPayloadBase* RequestPayload_Implementation(const UContext_ActionEntry* ActionEntry) const override;
	virtual FText GetDisplayName_Implementation() const override;
	// !IContext_Holder Implementation

private:
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;

	UFUNCTION(BlueprintCallable)
	UFunction* GetFunctionForAction(const FString&  ActionName, FName& ExpectedFunctionName) const;
};
