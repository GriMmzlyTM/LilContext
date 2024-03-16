// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Context_ActionSubsystem.h"

#include "Context_ActionPayloadBase.h"
#include "Actions/Context_Action.h"
#include "Actions/Context_ActionEntry.h"
#include "Interface/Context_Giver.h"
#include "Interface/Context_Holder.h"
#include "UI/Context_Menu.h"
#include "UI/Context_UIWidgetBase.h"

void UContext_ActionSubsystem::SetContextMenuInstance(UContext_Menu* ContextMenuInstance) {
	ContextMenu = ContextMenuInstance;
}

void UContext_ActionSubsystem::DisableContextSource(const EContext_ContextSource Source) {
	EnabledSources &= ~Source;
}

void UContext_ActionSubsystem::EnableContextSource(const EContext_ContextSource Source) {
	EnabledSources |= Source;
}

bool UContext_ActionSubsystem::CheckSourceEnabled(const EContext_ContextSource Source) {
	return (EnabledSources & Source) == Source;
}

void UContext_ActionSubsystem::ShowContextMenu(
	const TArray<FContextEntryPackage>& ContextEntries,
	const FVector WorldPosition) {

	if (!IsValid(ContextMenu)) return;

	// Prevent opening context for actors (world objects) if world context is disabled
	if (!CheckSourceEnabled(EContext_ContextSource::World)) return;

	ContextMenu->ShowMenu(WorldPosition, ContextEntries);
}

void UContext_ActionSubsystem::ShowUIContextMenu(
	const FVector2D ScreenPosition,
	const TArray<FContextEntryPackage>& ContextEntries) {
	if (!CheckSourceEnabled(EContext_ContextSource::UI) || !UIContextElement.IsValid()) return;

	if (const APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController(); IsValid(PC)) {
		ContextMenu->ShowMenuScreenSpace(ScreenPosition, ContextEntries, false);
	}
}

void UContext_ActionSubsystem::HideContextMenu() {
	if (!IsValid(ContextMenu)) return;
	ContextMenu->HideMenu();
}

void UContext_ActionSubsystem::HideUnfocusedContextMenu() {
	if (!IsValid(ContextMenu)) return;

	const bool HoveredState = ContextMenu->IsHovered();
	if (!HoveredState) {
		HideContextMenu();
	}
}

bool UContext_ActionSubsystem::ExecuteAction(TScriptInterface<IContext_Holder> ContextObject,
                                             const UContext_ActionEntry* Action,
                                             AActor* InstigatorActor) {
	// When we're about to execute, we want to run validations to make sure the action can actually be run at that moment
	AActor* ContextActor = Cast<AActor>(ContextObject.GetObject());
	if (!Action->RunActionValidations(InstigatorActor, ContextActor)) {
		return false;
	}
	
	const UContext_ActionPayloadBase* Payload = ContextObject->Execute_RequestPayload(ContextObject.GetObject(), Action);
	UContext_Action* ContextAction = NewObject<UContext_Action>(ContextObject.GetObject(), Action->Action);
	ContextAction->InstigatorActor = InstigatorActor;
	
	const UActorComponent* Component = Cast<UActorComponent>(ContextObject.GetObject());
	if (IsValid(Component)) {
		return ContextAction->ExecuteContextAction(Component->GetOwner(), Payload);
	}
	return ContextAction->ExecuteContextAction(ContextObject.GetObject(), Payload);
}

bool UContext_ActionSubsystem::CanExecuteEntry(
	const UObject* ContextObject,
	const UContext_ActionEntry* Entry) const {

	if(!IsValid(Entry)) {
		UE_LOG(LogContextSubsystem, Warning, TEXT("Invalid context entry passed to Action Subsystem"));
		return false;
	}
	
	const UObject* ContextHolder =
		ContextObject->Implements<UContext_Holder>() ?
			ContextObject :
			RetrieveValidContextHolderFromObject(ContextObject);

	if (!IsValid(ContextHolder)) {
		return false;
	}
	
	// If tags don't match (Similarly to abilities in gas) then the entry cannot be executed
	const FGameplayTagContainer Tags = IContext_Holder::Execute_GetTags(ContextHolder);
	if (Tags.HasAny(Entry->BlockingTags) || !Tags.HasAllExact(Entry->RequiredTags)) {
		return false;
	}
	return true;
}

UObject* UContext_ActionSubsystem::RetrieveValidContextHolderFromObjectNonConst(UObject* ContextObjectRoot) const {
	if (ContextObjectRoot->Implements<UContext_Holder>()) {
		return ContextObjectRoot;
	}

	// Actor component
	const AActor* Actor = Cast<AActor>(ContextObjectRoot);  
	if (IsValid(Actor)) {
		// return first component found
		TArray<UActorComponent*> Comps = Actor->GetComponentsByInterface(UContext_Holder::StaticClass());
		if (Comps.Num() > 0) {
			return Comps[0];
		}
	}

	return nullptr;
}

const UObject* UContext_ActionSubsystem::RetrieveValidContextHolderFromObject(const UObject* ContextObjectRoot) const {
	if (ContextObjectRoot->Implements<UContext_Holder>()) {
		return ContextObjectRoot;
	}

	// Actor component
	const AActor* Actor = Cast<AActor>(ContextObjectRoot);  
	if (IsValid(Actor)) {
		// return first component found
		TArray<UActorComponent*> Comps = Actor->GetComponentsByInterface(UContext_Holder::StaticClass());
		if (Comps.Num() > 0) {
			return Comps[0];
		}
	}

	return nullptr;
}

TSet<UContext_ActionEntry*> UContext_ActionSubsystem::GetValidContextEntriesForObject(
	const UObject* ContextObject) const {

	// Get the exact object that holds the context interface, and return an empty set if none.
	const UObject* ContextHolder = RetrieveValidContextHolderFromObject(ContextObject);
	if (!ensure(ContextHolder)) {
		return TSet<UContext_ActionEntry*>();
	}

	// Get all raw entries
	TSet<UContext_ActionEntry*> Entries = IContext_Holder::Execute_GetActionEntries(ContextHolder);
	Entries.Append(AggregateContextEntriesInTree(ContextHolder));

	// Only keep the valid entries - Checks tags
	TSet<UContext_ActionEntry*> ValidEntries;
	for (auto Entry : Entries) {
		if (CanExecuteEntry(ContextHolder, Entry)) {
			ValidEntries.Add(Entry);
		}
	}
	
	return ValidEntries;
}

UContext_ActionEntry* UContext_ActionSubsystem::GetPrimaryContextEntryForObject(const UObject* ContextObject) const {
	const UObject* ContextHolder = RetrieveValidContextHolderFromObject(ContextObject);
	if (!ensure(ContextHolder)) {
		return nullptr;
	}

	TArray<UContext_ActionEntry*> PrimaryEntries = IContext_Holder::Execute_GetPrimaryActionEntries(ContextHolder);
	for (const auto Entry : PrimaryEntries) {
		if (CanExecuteEntry(ContextHolder, Entry)) {
			return Entry;
		}
	}

	return nullptr;
}

UContext_ActionPayloadBase* UContext_ActionSubsystem::FindContextPayloadInTree(
	const UObject* ContextEntity,
	const TSubclassOf<UContext_ActionPayloadBase> PayloadClass,
	const int MaxDepth) const {

	if (!ContextEntity->Implements<UContext_Holder>()) {
		UE_LOG(LogContextSubsystem, Warning, TEXT("Attempting to iterate an object tree not implementing IContext_Holder! (%s)\n"
											"This function can only be called from a context holder to ensure it's only called once in a chain"),
											*GetFullName());
		return nullptr;;
	}

	int CurrentDepth = 1;
	UObject* CurrentRoot = GetNextObjectInTree(ContextEntity);
	while(IsValid(CurrentRoot) && CurrentDepth <= MaxDepth) {

		if (CurrentRoot->Implements<UContext_Giver>()) {
			UContext_ActionPayloadBase* FoundPayload = IContext_Giver::Execute_RequestContextPayload(CurrentRoot, PayloadClass);
			if (IsValid(FoundPayload)) {
				return FoundPayload;
			}
		}

		CurrentRoot = GetNextObjectInTree(CurrentRoot);
		CurrentDepth++;
	}

	return nullptr;
	
}

TSet<UContext_ActionEntry*> UContext_ActionSubsystem::AggregateContextEntriesInTree(
	const UObject* ContextEntity,
	const int MaxDepth) const {

	TSet<UContext_ActionEntry*> Entries;
	
	if (!ContextEntity->Implements<UContext_Holder>()) {
		UE_LOG(LogContextSubsystem, Warning, TEXT("Attempting to iterate an object tree not implementing IContext_Holder! (%s)\n"
											"This function can only be called from a context holder to ensure it's only called once in a chain"),
											*GetFullName());
		return Entries;;
	}
	
	int CurrentDepth = 1;
	UObject* CurrentRoot = GetNextObjectInTree(ContextEntity);
	while(IsValid(CurrentRoot) && CurrentDepth <= MaxDepth) {
		if (CurrentRoot->Implements<UContext_Giver>()) {
			IContext_Giver::Execute_GetGiverContextEntries(CurrentRoot, Entries);
		}

		CurrentRoot = GetNextObjectInTree(CurrentRoot);
		CurrentDepth++;
	}

	return Entries;
}

UContext_ActionEntry* UContext_ActionSubsystem::FindPrimaryContextEntryInTree(
	const UObject* ContextEntity,
	const int MaxDepth) const {
	if (!ContextEntity->Implements<UContext_Holder>()) {
		UE_LOG(LogContextSubsystem, Warning, TEXT("Attempting to iterate an object tree not implementing IContext_Holder! (%s)\n"
											"This function can only be called from a context holder to ensure it's only called once in a chain"),
											*GetFullName());
		return nullptr;;
	}
	
	int CurrentDepth = 1;
	UObject* CurrentRoot = GetNextObjectInTree(ContextEntity);
	while(IsValid(CurrentRoot) && CurrentDepth <= MaxDepth) {
		if (CurrentRoot->Implements<UContext_Giver>()) {
			UContext_ActionEntry* ActionEntry = IContext_Giver::Execute_GetPrimaryContextEntry(CurrentRoot);
			if (IsValid(ActionEntry)) {
				return ActionEntry;
			}
		}

		CurrentRoot = GetNextObjectInTree(CurrentRoot);
		CurrentDepth++;
	}

	return nullptr;
}

UObject* UContext_ActionSubsystem::GetNextObjectInTree(const UObject* ContextEntity) const {

	if (ContextEntity == GetWorld()) return nullptr;
	
	if (const UUserWidget* Widget = Cast<UUserWidget>(ContextEntity)) {
		return Widget->GetParent();
	}

	if (const UActorComponent* Comp = Cast<UActorComponent>(ContextEntity)) {
		return Comp->GetOwner();
	}

	return ContextEntity->GetOuter();
	
}
