// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Context_Action.h"

#include "Context_ActionPayloadBase.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DataValidation.h"

AActor* UContext_Action::GetInstigator() {
	return InstigatorActor;
}

AHUD* UContext_Action::GetLocalPlayerHUD() const {
	const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	return PC->GetHUD();
}

APlayerController* UContext_Action::GetLocalPlayerControllerForAction() const {
	return GEngine->GetFirstLocalPlayerController(GetWorld());
}

UWorld* UContext_Action::GetWorld() const {
	if(IsValid(InstigatorActor)) {
		return InstigatorActor->GetWorld();
	}

	// Special case for behavior tree nodes in the editor
	if (Cast<UPackage>(GetOuter()) != nullptr)
	{
		// GetOuter should return a UPackage and its Outer is a UWorld
		return Cast<UWorld>(GetOuter()->GetOuter());
	}

	if (GetOuter() == nullptr) return nullptr;
	
	return GetOuter()->GetWorld();
}

void UContext_Action::CommitPayload(const UContext_ActionPayloadBase* Payload) {
}

#if WITH_EDITOR
EDataValidationResult UContext_Action::IsDataValid(FDataValidationContext& Context) const {
	const EDataValidationResult BaseResult = UObject::IsDataValid(Context);
	
	if (!IsValid(PayloadClass)) {
		Context.AddError(FText::FromString(TEXT("Payload class must be set to a valid payload type")));
	}

	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : BaseResult;
}
#endif