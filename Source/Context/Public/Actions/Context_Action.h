// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Context_ActionPayloadBase.h"
#include "Context_SystemComponent.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "Context_Action.generated.h"

class UContext_ActionPayloadBase;
class IContext_Holder;
/**
 * An action that is executed by a context menu or context execution of some kind
 * This action should be fairly self contained
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CONTEXT_API UContext_Action : public UObject {
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Context|Action|Payload")
	TSubclassOf<UContext_ActionPayloadBase> PayloadClass;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* InstigatorActor;

	/**
	 * The logic that gets called when this context action is executed 
	 * @param ContextHolder  The object that this action interacts with, or fetches data from
	 * @param Payload The payload to pass the action, if any
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool ExecuteContextAction(UObject* ContextHolder, const UContext_ActionPayloadBase* Payload);
	virtual bool ExecuteContextAction_Implementation(UObject* ContextHolder, const UContext_ActionPayloadBase* Payload) {
		// is no payload is set, we expect a nullptr
		if (!IsValid(PayloadClass) && Payload != nullptr) {
			return false;
		}

		// if our payload is different than the desired payload, fail
		if (Payload->GetClass() != PayloadClass) {
			UE_LOG(LogContextSystem, Error, TEXT("Incorrect payload type passed to %s. Expected %s but got %s"),
				*GetName(),
				*PayloadClass->GetName(),
				*Payload->GetClass()->GetName())
			return false;
		}
		
		return true;
	}

	UFUNCTION(BlueprintCallable, Category = "Context")
	AActor* GetInstigator();
	
	/**
	 * Gets and returns the HUD belonging to the local player actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Action|Helper")
	AHUD* GetLocalPlayerHUD() const;

	/**
	 * Fetches the player controller owned by the local player
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Action|Helper")
	APlayerController* GetLocalPlayerControllerForAction() const;

	/**
	 * Helper for exposing GetWorld, which would not otherwise be usable 
	 */
	UFUNCTION(BlueprintCallable, Category = "Context|Action|Helper")
	virtual UWorld* GetWorld() const override;

	UFUNCTION()
	void CommitPayload(const UContext_ActionPayloadBase* Payload);
	
private:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
