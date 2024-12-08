// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/Context_ActionSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Context_Menu.generated.h"

class UVerticalBox;
class UCommonButtonBase;
class UContext_ActionEntry;
class IContext_Holder;
class UContext_EntryButton;
/**
 * A context menu is a container for all context entry buttons.
 *
 * The ContextMenu sets up an EntryButton for each entry provided, and displays it on the screen. 
 */
UCLASS()
class CONTEXT_API UContext_Menu : public UUserWidget {
	GENERATED_BODY()

	/// Parameters
	
	/**
	 * If true, this will force the context menu to stay where it was spawned.
	 * If false, the menu will continue being represented in the world, and move on screen as the player does
	 * This does *not* make the menu follow the context entity, because that would make it hard to click
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Context|Settings")
	bool bShouldBeStaticOnScreen = false;
	
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess=true), Category = "Context|Setup")
	TSubclassOf<UButton> EntryButtonTemplate;

	/// widget elements
	
	UPROPERTY(meta=(BindWidget))
	TWeakObjectPtr<UVerticalBox> ContextButtonContainer;
	
	UPROPERTY()
	TArray<UContext_EntryButton*> ContextEntryButtons;

	/// Fields
	
	UPROPERTY()
	FVector WorldLocation = FVector::Zero();
	
public:
	/**
	 * Shows the menu with generated menu items based on provided values
	 * @param WorldSpawnLocation Where the menu should show on the screen
	 * @param ContextHolder The context object that this menu is for
	 * @param ContextEntries The entries to use when generating buttons
	 */
	UFUNCTION(BlueprintCallable)
	void ShowMenu(
		const FVector WorldSpawnLocation,
		const TArray<FContextEntryPackage>& ContextEntries,
		const bool bRemoveDPIScale = true);

	UFUNCTION(BlueprintCallable)
	void ShowMenuScreenSpace(
		const FVector2D ScreenSpaceLocation,
		const TArray<FContextEntryPackage>& ContextEntries,
		const bool bRemoveDPIScale = true);
	
	/**
	 * Hide the menu and clear all entries
	 */
	UFUNCTION(BlueprintCallable)
	void HideMenu();

	UFUNCTION()
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION()
	void ShowMenuInternal(const FVector2D ScreenSpaceLocation,
	                      const TArray<FContextEntryPackage>& ContextEntryPackage,
	                      const bool bRemoveDPIScale = true);
};
