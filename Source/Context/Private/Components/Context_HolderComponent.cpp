// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Context_HolderComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Context_ActionPayloadBase.h"
#include "Actions/Context_Action.h"
#include "Actions/Context_ActionEntry.h"
#include "Misc/DataValidation.h"

// Sets default values for this component's properties
UContext_HolderComponent::UContext_HolderComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

FGameplayTagContainer UContext_HolderComponent::GetTags_Implementation() const {

	FGameplayTagContainer OwnedTags;
	
	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
	if (ASI)
	{
		ASI->GetAbilitySystemComponent()->GetOwnedGameplayTags(OwnedTags);
	}

	if (OwnedTags.IsEmpty()) {
		OwnedTags.AppendTags(DefaultTags);
	}
	
	return OwnedTags;
}

FVector UContext_HolderComponent::GetPosition_Implementation() const {
	return GetOwner()->GetActorLocation();
}

TSet<UContext_ActionEntry*> UContext_HolderComponent::GetActionEntries_Implementation() const {
	return ContextEntries;
}

FText UContext_HolderComponent::GetDisplayName_Implementation() const {
	if (bDisplayNameOverride) {
		return DisplayNameOverride;
	}

	if (!DisplayName.IsEmptyOrWhitespace()) {
		return DisplayName;
	}

	return FText::FromString("MISCONFIGURED_CONTEXT_OBJECT");
}

const UContext_ActionPayloadBase* UContext_HolderComponent::RequestPayloadOfType_Implementation(
	TSubclassOf<UContext_ActionPayloadBase> PayloadType) const {

	TSet<UContext_ActionEntry*> Entries = ContextEntries.Union(TSet(PrimaryContextEntryPriority));
	for (const auto Entry : Entries) {
		if (Entry->Action->GetDefaultObject<UContext_Action>()->PayloadClass == PayloadType) {
			return Execute_RequestPayload(this, Entry);
		}
	}
	return nullptr;
}

TArray<UContext_ActionEntry*> UContext_HolderComponent::GetPrimaryActionEntries_Implementation() const {
	return PrimaryContextEntryPriority;
}

const UContext_ActionPayloadBase* UContext_HolderComponent::RequestPayload_Implementation(
	const UContext_ActionEntry* ActionEntry) const {
	
	FName ExpectedFunctionName;
	UFunction* PayloadFunc = GetFunctionForAction(ActionEntry->ActionName.ToString(), ExpectedFunctionName);
	
	// Function should be valid, if not then it hasn't been defined
	if (!IsValid(PayloadFunc)) {
		UE_LOG(LogContextComponent, Warning, TEXT("Could not find payload function %ls on object %ls for action %ls"),
			*ExpectedFunctionName.ToString(),
			*GetName(),
			*ActionEntry->ActionName.ToString())
		return nullptr;
	}

	// Execute function and get return value
	struct FPayloadFuncParams {
		UContext_ActionPayloadBase* ReturnValue;
	};
	
	FPayloadFuncParams Params;
	GetOwner()->ProcessEvent(PayloadFunc, &Params);
	
	return Params.ReturnValue;
}

void UContext_HolderComponent::SetDisplayName(FText Name) {
	DisplayName = Name;
}

// Called when the game starts
void UContext_HolderComponent::BeginPlay() {
	Super::BeginPlay();
	
}

EDataValidationResult UContext_HolderComponent::IsDataValid(FDataValidationContext& Context) const {
	const EDataValidationResult BaseResult = Super::IsDataValid(Context);

	// We only want to run this validation on AActors/when its attached. That's because we're validating
	// the the required functions exist
	const AActor* OwningActor = GetOwner();
	if (!IsValid(OwningActor)) {
		return EDataValidationResult::Valid;
	}

	////////////
	//// Validate all context entries
	TSet<UContext_ActionEntry*> EntriesToValidate = ContextEntries.Union(TSet(PrimaryContextEntryPriority));
	for (const auto Entry : EntriesToValidate) {
		const UContext_Action* BaseAction = Entry->Action.GetDefaultObject();

		if (!IsValid(BaseAction->PayloadClass)) continue;
		
		FName ExpectedFunctionName;
		const UFunction* Function = GetFunctionForAction(Entry->ActionName.ToString(), ExpectedFunctionName);
		
		// PAYLOAD FUNCTION SHOULD EXIST FOR CONTEXT ENTRY
		if (!IsValid(Function)) {
			FText WarningMsg = FText::FromString(FString::Printf(TEXT("Function %s should be created to retrieve %s for action"),
				*ExpectedFunctionName.ToString(),
				*Entry->ActionName.ToString()));
			Context.AddWarning(WarningMsg);
			continue;
		}

		// PAYLOAD FUNCTION MUST HAVE VALID RETURN
		bool bHasValidReturn = false;
		for(TFieldIterator<FObjectProperty> It(Function); It; ++It) {
			if (It->HasAnyPropertyFlags(CPF_ReturnParm | CPF_OutParm) && It->PropertyClass == BaseAction->PayloadClass) {
				bHasValidReturn = true;
				break;
			}
		}
		
		if (!bHasValidReturn) {
			FText WarningMsg = FText::FromString(FString::Printf(
				TEXT("Function %s should return %s"),
				*ExpectedFunctionName.ToString(),
				*BaseAction->PayloadClass->GetName()));
		
			Context.AddError(WarningMsg);
		}
		
		// PAYLOAD FUNCTION SHOULD BE CONST
		if (!(Function->FunctionFlags & FUNC_Const)) {
			FText WarningMsg = FText::FromString(FString::Printf(
				TEXT("Function %s should be const"),
				*ExpectedFunctionName.ToString()));
			Context.AddError(WarningMsg);
		}

	}

	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : BaseResult;
	
}

UFunction* UContext_HolderComponent::GetFunctionForAction(const FString& ActionName, FName& ExpectedFunctionName) const {

	const AActor* OwningActor = GetOwner();
	if (!IsValid(OwningActor)) {
		return nullptr;
	}
	
	// ~name stuff
	// really funky, but it basically just copies how blueprint strings are named by default
	const FString FunctionNameBase = FString::Printf(TEXT("%s_%s"), *ContextPayloadFunctionPrefix, *ActionName);
	const FName DisplayFunctionName = FName(
		FName::NameToDisplayString(
			FunctionNameBase,
			false));
	ExpectedFunctionName = FName(FunctionNameBase);
	// ~name stuff end

		
	UFunction* FunctionForAction = OwningActor->FindFunction(DisplayFunctionName);
	if (!IsValid(FunctionForAction)) {
		return OwningActor->FindFunction(ExpectedFunctionName);
	}

	return FunctionForAction;
}
