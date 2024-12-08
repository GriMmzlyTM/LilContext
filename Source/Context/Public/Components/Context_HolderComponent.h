// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/Context_Giver.h"
#include "Interface/Context_Holder.h"
#include "Context_HolderComponent.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogContextComponent, Log, All);

UCLASS(ClassGroup=(Custom), Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class CONTEXT_API UContext_HolderComponent : public UActorComponent, public IContext_Holder {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(InlineEditConditionToggle))
	bool bDisplayNameOverride;
	
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bDisplayNameOverride"))
	FText DisplayNameOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Context|Holder|Entries")
	TSet<UContext_ActionEntry*> ContextEntries;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Context|Holder|Entries")
	TArray<UContext_ActionEntry*> PrimaryContextEntryPriority;

	UPROPERTY(VisibleAnywhere)
	FText DisplayName;

	/**
	 * These are the default tags owned by this component.
	 * This is ONLY used if there is no ability system component attached to an actor. The point of this is to allow
	 * object that don't require GAS, but still have a context, to be used.
	 * I.E. floor tiles, doors, walls, etc.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Context|Holder|Data")
	FGameplayTagContainer DefaultTags;
	
public:
	// Sets default values for this component's properties
	UContext_HolderComponent();

	// IContext_Holder interface BEGIN
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	virtual FVector GetPosition_Implementation() const override;
	
	virtual TSet<UContext_ActionEntry*> GetActionEntries_Implementation() const override;
	
	virtual FText GetDisplayName_Implementation() const override;
	
	virtual const UContext_ActionPayloadBase* RequestPayloadOfType_Implementation(TSubclassOf<UContext_ActionPayloadBase> PayloadType) const override;

	virtual TArray<UContext_ActionEntry*> GetPrimaryActionEntries_Implementation() const override;
	
	virtual const UContext_ActionPayloadBase* RequestPayload_Implementation(const UContext_ActionEntry* ActionEntry) const override;
	
	// IContext_Holder interface END
	
	UFUNCTION(BlueprintCallable, Category = "Skill|Resource|Context")
	void SetDisplayName(FText Name);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	// Format that is enforced for function names used to get payloads for actions
	UPROPERTY(VisibleAnywhere)
	FString ContextPayloadFunctionPrefix = TEXT("GetPayload");

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

	UFUNCTION(BlueprintCallable)
	UFunction* GetFunctionForAction(const FString&  ActionName, FName& ExpectedFunctionName) const;

};
