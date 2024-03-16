// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\UI\Context_UIWidgetBase.h"

#include "Actions/Context_Action.h"
#include "Actions/Context_ActionEntry.h"
#include "Actions/Context_ActionSubsystem.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Context_HolderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DataValidation.h"

FReply UContext_UIWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	UContext_ActionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UContext_ActionSubsystem>();
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {
		// only proceed if UI context is enabled
		if (Subsystem->CheckSourceEnabled(EContext_ContextSource::UI)) {
			Subsystem->UIContextElement = this;

			const FVector2D MousePos = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this).AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

			const TSet<UContext_ActionEntry*> ActionEntries = Subsystem->GetValidContextEntriesForObject(this);

			const FContextEntryPackage EntryPackage(this, ActionEntries);
			TArray<FContextEntryPackage> EntryPackages;
			EntryPackages.Add(EntryPackage);
			Subsystem->ShowUIContextMenu(MousePos, EntryPackages);
			return FReply::Handled();;
		}
	}
	Subsystem->HideContextMenu();
	return FReply::Unhandled();
}

FReply UContext_UIWidgetBase::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent) {
	
	UContext_ActionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UContext_ActionSubsystem>();
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {
		// only proceed is UI context is enabled 
		if (!Subsystem->CheckSourceEnabled(EContext_ContextSource::UI)) return FReply::Unhandled();

		Subsystem->UIContextElement = this;

		const UContext_ActionEntry* ActionEntry = Subsystem->GetPrimaryContextEntryForObject(this);
		if (IsValid(ActionEntry)) {
			return Subsystem->ExecuteAction(this, ActionEntry, GetOwningPlayerPawn()) ? FReply::Handled() : FReply::Unhandled();
		}
	}
	return FReply::Unhandled();
}

void UContext_UIWidgetBase::GiveTag(FGameplayTagContainer Tags) {
	DefaultTags.AppendTags(Tags);
}

void UContext_UIWidgetBase::RemoveTag(FGameplayTagContainer Tags) {
	DefaultTags.RemoveTags(Tags);
}

void UContext_UIWidgetBase::ClearTags() {
	DefaultTags.Reset();
}

FVector UContext_UIWidgetBase::GetPosition_Implementation() const {
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(const_cast<UContext_UIWidgetBase*>(this));

	FVector WorldPos;
	FVector WorldDir;
	
	UGameplayStatics::DeprojectScreenToWorld(PlayerController, MousePos, WorldPos, WorldDir);
	return WorldPos;
}

TSet<UContext_ActionEntry*> UContext_UIWidgetBase::GetActionEntries_Implementation() const {
	return ContextEntries;
}

const UContext_ActionPayloadBase* UContext_UIWidgetBase::RequestPayloadOfType_Implementation(
	TSubclassOf<UContext_ActionPayloadBase> PayloadType) const {
		TSet<UContext_ActionEntry*> Entries = ContextEntries.Union(TSet(PrimaryContextEntryPriority));
		for (const auto Entry : Entries) {
			if (Entry->Action->GetDefaultObject<UContext_Action>()->PayloadClass == PayloadType) {
				return Execute_RequestPayload(this, Entry);
			}
		}
		return nullptr;
}

const UContext_ActionPayloadBase* UContext_UIWidgetBase::RequestPayload_Implementation(
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

	UContext_UIWidgetBase* SelfWidget = const_cast<UContext_UIWidgetBase*>(this);
	FPayloadFuncParams Params;
	SelfWidget->ProcessEvent(PayloadFunc, &Params);

	return Params.ReturnValue;
}

TArray<UContext_ActionEntry*> UContext_UIWidgetBase::GetPrimaryActionEntries_Implementation() const {
	return PrimaryContextEntryPriority;
}

FText UContext_UIWidgetBase::GetDisplayName_Implementation() const {
	return FText::FromString(TEXT("UNHANDLED_UI_NAME"));
}

UFunction* UContext_UIWidgetBase::GetFunctionForAction(const FString& ActionName, FName& ExpectedFunctionName) const {
	
	// ~name stuff
	// really funky, but it basically just copies how blueprint strings are named by default
	const FString FunctionNameBase = FString::Printf(TEXT("GetPayload_%s"), *ActionName);
	const FName DisplayFunctionName = FName(
		FName::NameToDisplayString(
			FunctionNameBase,
			false));
	ExpectedFunctionName = FName(FunctionNameBase);
	// ~name stuff end
		
	UFunction* FunctionForAction = FindFunction(DisplayFunctionName);
	return FunctionForAction;
}

EDataValidationResult UContext_UIWidgetBase::IsDataValid(FDataValidationContext& Context) const {
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
				*BaseAction->PayloadClass->GetName()));
			Context.AddWarning(WarningMsg);
			continue;
		}

		// PAYLOAD FUNCTION MUST HAVE VALID RETURN
		bool bHasValidReturn = false;
		for(TFieldIterator<FObjectProperty> It(Function); It; ++It) {
			UE_LOG(LogTemp, Warning, TEXT("%s"), *It->GetName());
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

	const EDataValidationResult BaseResult = Super::IsDataValid(Context);
	
	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : BaseResult;
}
