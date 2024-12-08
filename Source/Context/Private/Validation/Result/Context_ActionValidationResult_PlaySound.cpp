// Fill out your copyright notice in the Description page of Project Settings.


#include "Validation/Result/Context_ActionValidationResult_PlaySound.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/DataValidation.h"
#include "Sound/SoundCue.h"

void UContext_ActionValidationResult_PlaySound::OnResultActionStart_Implementation(AActor* Entity) {
	UGameplayStatics::PlaySound2D(Entity, SoundCueToPlay);
}

#if WITH_EDITOR
EDataValidationResult UContext_ActionValidationResult_PlaySound::IsDataValid(FDataValidationContext& Context) const {

	if (!IsValid(SoundCueToPlay)) {
		Context.AddError(FText::FromString("Action Result PlaySound must be provided a sound cue"));
	}
	
	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif