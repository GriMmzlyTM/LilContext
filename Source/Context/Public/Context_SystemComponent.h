// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Context_SystemComponent.generated.h"

class UContext_ActionSubsystem;
class UInputMappingContext;
class UContext_ActionEntry;
class UInputAction;

DEFINE_LOG_CATEGORY_STATIC(LogContextSystem, Log, All);

/**
 * Context system allowing an actor (The player, most likely) to interact with the Context library by responding to
 * input actions.
 */
UCLASS()
class CONTEXT_API UContext_SystemComponent : public UActorComponent {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ContextMenuTemplate;
	
	/**
	 * The input that will be used to activate the context menu
	 * It is expected that this Input Action has been added to your Mapping Context, which is expected to be setup on your player character
	 * This component will *not* set up the mapping context. We are simply binding to this action. 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Context|System|Input", meta=(AllowPrivateAccess = true))
	const UInputAction* OpenContextInputAction;

	/**
	 * The input action that is expected to close the context menu - Likely left mouse button
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Context|System|Input", meta=(AllowPrivateAccess = true))
	const UInputAction* CloseContextInputAction;
	
	/**
	 * Default actions that should appear on *every* IContext_Holder.
	 * Please ensure that any actions here are entirely entity agnostic, and can work on any actor.
	 * A good example would be a "Examine" context, which shows a description of thing being checked. 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Context|System|Actions", meta=(AllowPrivateAccess = true))
	TArray<UContext_ActionEntry*> DefaultActions;

	UPROPERTY()
	UContext_ActionSubsystem* ActionSubsystem;
	
public:
	// Sets default values for this actor's properties
	UContext_SystemComponent();

private:
	UFUNCTION()
	APlayerController* GetCurrentActorController() const;
	
protected:
	UFUNCTION()
	void OpenContextMenu();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
};
