// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Context_ActionPayloadBase.generated.h"

/**
 * Base class for all action payloads
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CONTEXT_API UContext_ActionPayloadBase : public UDataAsset {
	GENERATED_BODY()
};
