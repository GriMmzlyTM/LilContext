// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Context_Menu.h"

#include "Actions/Context_ActionSubsystem.h"
#include "Components/VerticalBox.h"
#include "UI/Context_EntryButton.h"

void UContext_Menu::ShowMenu(
	const FVector WorldSpawnLocation,
	const TArray<FContextEntryPackage>& ContextEntries,
	const bool bRemoveDPIScale) {

	WorldLocation = WorldSpawnLocation;
	
	FVector2D ScreenLocation;
	GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(WorldLocation,ScreenLocation);
	ShowMenuInternal(ScreenLocation, ContextEntries, bRemoveDPIScale);
}

void UContext_Menu::ShowMenuScreenSpace(
	const FVector2D ScreenSpaceLocation,
	const TArray<FContextEntryPackage>& ContextEntries,
	const bool bRemoveDPIScale) {

	WorldLocation = FVector::Zero();
	ShowMenuInternal(ScreenSpaceLocation, ContextEntries, bRemoveDPIScale);
}

void UContext_Menu::ShowMenuInternal(
	const FVector2D ScreenSpaceLocation,
    const TArray<FContextEntryPackage>& ContextEntryPackage,
    const bool bRemoveDPIScale) {
	
	SetPositionInViewport(ScreenSpaceLocation, bRemoveDPIScale);

	ContextEntryButtons.Empty();
	ContextButtonContainer->ClearChildren();

	// loop through object found
	for (const auto [ContextHolder, ContextEntries] : ContextEntryPackage) {
		for (const auto ContextEntry : ContextEntries) {
			//UContext_EntryButton* ContextEntryButtonInstance = CreateWidget<UContext_EntryButton>(this, EntryButtonTemplate);
			//ContextEntryButtonInstance->Setup(GetOwningPlayerPawn(), ContextEntry, ContextHolder);
			//ContextButtonContainer->AddChildToVerticalBox(ContextEntryButtonInstance);
			//ContextEntryButtons.Add(ContextEntryButtonInstance);	
		}
	}
	
	SetVisibility(ESlateVisibility::Visible);
}

void UContext_Menu::HideMenu() {
	for(const auto EntryButton : ContextEntryButtons) {
		EntryButton->RemoveFromParent();
	}

	ContextEntryButtons.Empty();
	ContextButtonContainer->ClearChildren();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UContext_Menu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bShouldBeStaticOnScreen || WorldLocation == FVector::Zero()) return;

	FVector2D ScreenLocation;
	GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(WorldLocation,ScreenLocation);
	SetPositionInViewport(ScreenLocation, true);
}

