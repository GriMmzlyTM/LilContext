// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Context_EntryButton.h"
#include "Interface/Context_Holder.h"
#include "CommonTextBlock.h"
#include "Actions/Context_Action.h"
#include "Actions/Context_ActionEntry.h"
#include "Actions/Context_ActionSubsystem.h"

void UContext_EntryButton::Setup(AActor* Instigator, const UContext_ActionEntry* ContextEntry, const TScriptInterface<IContext_Holder> ContextHolder) {
	ContextActionName->SetText(ContextEntry->ActionName);

	if (ContextEntry->bDisplayEntityName) {
		ContextEntityName->SetVisibility(ESlateVisibility::Visible);
		ContextEntityName->SetText(IContext_Holder::Execute_GetDisplayName(ContextHolder.GetObject()));
	} else {
		ContextEntityName->SetVisibility(ESlateVisibility::Collapsed);
	}

	ContextAction = ContextEntry;
	InstigatingActor = Instigator;
	ContextObject = ContextHolder;
	OnButtonBaseClicked.AddDynamic(this, &UContext_EntryButton::ActionSelected);
}

void UContext_EntryButton::ActionSelected(UCommonButtonBase* Button) {
	UContext_ActionSubsystem* ActionSubsystem = GetGameInstance()->GetSubsystem<UContext_ActionSubsystem>();
	ActionSubsystem->ExecuteAction(ContextObject, ContextAction, InstigatingActor);
	ActionSubsystem->HideContextMenu();
}
