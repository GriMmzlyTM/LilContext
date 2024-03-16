// Fill out your copyright notice in the Description page of Project Settings.


#include "Validation/Result/Context_ActionValidationResult_ShowWorldDisplayMessage.h"

#include "UMGC_Subsystem.h"
#include "Helpers/UMGC_EntityUIStatics.h"

void UContext_ActionValidationResult_ShowWorldDisplayMessage::OnResultActionStart_Implementation(AActor* Entity) {
	UUMGC_Subsystem* Subsystem = Entity->GetGameInstance()->GetSubsystem<UUMGC_Subsystem>();

	Subsystem->DisplayInGameNotification(DisplayText, UUMGC_EntityUIStatics::GetActorCenterLocation(Entity));
}
