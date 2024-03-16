// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Context_ActionSubsystem.generated.h"

class UContext_ActionPayloadBase;
class UContext_UIWidgetBase;
class UContext_Menu;
class UContext_ActionEntry;
class UContext_Action;
class IContext_Holder;

DEFINE_LOG_CATEGORY_STATIC(LogContextSubsystem, Log, All);

/**
 * Specific context sources which represent something the context system can interact with to retrieve contexts
 * The reason for multiple sources is to specify which source we're trying to access, or disable certain sources
 * such as when an overlay UI is open, or the player is dead.
 */
UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EContext_ContextSource : uint8 {
	NONE	=  0 UMETA(Hidden),
	World	= 1,
	UI		= 2,
};
ENUM_CLASS_FLAGS(EContext_ContextSource);


USTRUCT(BlueprintType)
/**
 * Payload for passing multiple/every context entry associated with a specific context holder. 
 */
struct FContextEntryPackage {
	GENERATED_BODY()

	UPROPERTY()
	TScriptInterface<IContext_Holder> ContextHolder;

	UPROPERTY()
	TSet<UContext_ActionEntry*> ContextEntries;
};

/**
 * 
 */
UCLASS()
class CONTEXT_API UContext_ActionSubsystem : public UGameInstanceSubsystem {
	GENERATED_BODY()

	UPROPERTY()
	UContext_Menu* ContextMenu;

	UPROPERTY(meta = (Bitmask, BitmaskEnum=EContext_EnabledContextSource))
	EContext_ContextSource EnabledSources;

	
public:

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UContext_UIWidgetBase> UIContextElement;

	////////
	/// ~CONTEXT SOURCE
	
	/**
	 * Flags the desired context source as disable, disallowing the system from interacting with it if called
	 * @param Source The source to disable
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|State")
	void DisableContextSource(EContext_ContextSource Source);

	/**
	 * Flags the desired context source as enabled, allowing the system to interact with it if called
	 * @param Source The source to enable
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|State")
	void EnableContextSource(EContext_ContextSource Source);

	/**
	 * Validates if a source is currently active
	 * @param Source The source enum to check
	 * @return If the source is active
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|State")
	bool CheckSourceEnabled(EContext_ContextSource Source);

	////////
	/// ~UI CODE
	
	/**
	 * Sets the context menu for this subsystem to use. It's very important to call this at the beginning of the game
	 * @param ContextMenuInstance The context menu to pass
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|UI")
	void SetContextMenuInstance(UContext_Menu* ContextMenuInstance);
	
	/**
	 * Shows the context menu UI on the screen
	 * @param ContextEntries Entries that will be used to populate the menu
	 * @param WorldPosition The position in the world to display/spawn the menu. If not set, uses the mouse's current position at spawn time
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|UI")
	void ShowContextMenu(
		const TArray<FContextEntryPackage>& ContextEntries,
		FVector WorldPosition);

	/**
	 * Shows the context menu UI on the screen, specifically for UI elements
	 * @param ScreenPosition The screen position to display the menu
	 * @param ContextEntries The context entries to pass to th emenu
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|UI")
	void ShowUIContextMenu(
		FVector2D ScreenPosition,
		const TArray<FContextEntryPackage>& ContextEntries);
	
	/**
	 * Hides the context menu, if it is currently visible
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|UI")
	void HideContextMenu();

	/**
	 * Hides the context menu, but only if it is not currently focused or moused over.
	 * This prevents the menu from closing if the player is interacting with it
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|UI")
	void HideUnfocusedContextMenu();

	///////
	/// ~CONTEXT ACTION EXECUTION
	
	/**
	 * Execute the provided action 
	 * @param ContextObject 
	 * @param Action 
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|Action")
	bool ExecuteAction(TScriptInterface<IContext_Holder> ContextObject, const UContext_ActionEntry* Action, AActor* InstigatorActor);

	/**
	 * Execute the provided action 
	 * @param ContextObject 
	 * @param Action 
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|Info")
	bool CanExecuteEntry(const UObject* ContextObject, const UContext_ActionEntry* Entry) const;


	////////
	/// ~RETRIEVE DATA FROM CONTEXT OBJECT

	UFUNCTION(BlueprintCallable)
	UObject* RetrieveValidContextHolderFromObjectNonConst(UObject* ContextObjectRoot) const;

	/// Gets the first valid context holding object within the hierarchy/tree of the provided object
	/// This includes actor components, but not UI hierarchy
	/// @param ContextObjectRoot 
	/// @return 
	UFUNCTION(BlueprintCallable)
	const UObject* RetrieveValidContextHolderFromObject(const UObject* ContextObjectRoot) const;

	/// Gets all valid context entries for the provided object. This means all entries that can be executed
	UFUNCTION(BlueprintCallable)
	TSet<UContext_ActionEntry*> GetValidContextEntriesForObject(const UObject* ContextObject) const;

	UFUNCTION(BlueprintCallable)
	UContext_ActionEntry* GetPrimaryContextEntryForObject(const UObject* ContextObject) const;
	
	////////
	/// ~ITERATE OVER CONTEXT OBJECTS
	
	/**
	 * Iterates through every parent capable of providing a context or payload, and checks if it has the desired payload
	 * @param ContextEntity The context object used as root. Will NOT search this entity for payloads.
	 * @param PayloadClass The class to search for
	 * @param MaxDepth The maximum depth to go in the tree (Lower depth = better performance)
	 * @return Payload, if found. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|Action")
	UContext_ActionPayloadBase* FindContextPayloadInTree(
		const UObject* ContextEntity,
		TSubclassOf<UContext_ActionPayloadBase> PayloadClass,
		const int MaxDepth = 10) const;

	/**
	 * Iterated over every parent and aggregates all context entries they provide
	 * @param ContextEntity The entity to use as a root. This entities entries will NOT be added to the return value
	 * * @param MaxDepth The maximum depth to go in the tree (Lower depth = better performance)
	 * @return Set possibly containing additional entries
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|Action")
	TSet<UContext_ActionEntry*> AggregateContextEntriesInTree(const UObject* ContextEntity, const int MaxDepth = 10) const;

	UFUNCTION(BlueprintCallable, Category = "Context|Subsystem|Action")
	UContext_ActionEntry* FindPrimaryContextEntryInTree(const UObject* ContextEntity, const int MaxDepth = 10) const;
	
private:
	/**
	 * Iterated over every parent and aggregates all context entries they provide
	 * @param ContextEntity The entity to use as a root. This entities entries will NOT be added to the return value
	 * @return Set possibly containing additional entries
	 */
	UFUNCTION()
	UObject* GetNextObjectInTree(const UObject* ContextEntity) const;
	
};
