// Fill out your copyright notice in the Description page of Project Settings.


#include "Context_SystemComponent.h"

#include "EnhancedInputComponent.h"
#include "Actions/Context_ActionSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "Interface/Context_Holder.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Context_Menu.h"


// Sets default values
UContext_SystemComponent::UContext_SystemComponent() {
}

void UContext_SystemComponent::BeginPlay() {
	Super::BeginPlay();

	ensure(OpenContextInputAction);
	ensure(CloseContextInputAction);
	ensure(ContextMenuTemplate);
	
	AActor* ContextOwner = GetOwner();
	if (ContextOwner->HasAuthority()) {
		UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(ContextOwner->InputComponent);

		ActionSubsystem = ContextOwner->GetGameInstance()->GetSubsystem<UContext_ActionSubsystem>();
		
		if (!IsValid(EnhancedInputComp)) {
			UE_LOG(LogContextSystem, Warning, TEXT("Context plugin only supports the EnhancedInputSystem.\n"
										  "Owning actor does not seem to have a valid input system, or is not Input Enabled."));
			return;
		}

		EnhancedInputComp->BindAction(OpenContextInputAction, ETriggerEvent::Completed, this, &UContext_SystemComponent::OpenContextMenu);
		EnhancedInputComp->BindAction(CloseContextInputAction, ETriggerEvent::Completed, ActionSubsystem, &UContext_ActionSubsystem::HideUnfocusedContextMenu);

		UContext_Menu* ContextMenu = CreateWidget<UContext_Menu>(GetWorld(), ContextMenuTemplate);
		ContextMenu->AddToViewport();
		ContextMenu->HideMenu();

		ActionSubsystem->SetContextMenuInstance(ContextMenu);
		ActionSubsystem->EnableContextSource(EContext_ContextSource::World);
		ActionSubsystem->EnableContextSource(EContext_ContextSource::UI);
	}
}

void UContext_SystemComponent::OpenContextMenu() {
	const APlayerController* PlayerController = GetCurrentActorController();
	if (!IsValid(PlayerController)) {
		return;
	}
	
	// world context is enabled - we want to use world items 
	if(ActionSubsystem->CheckSourceEnabled(EContext_ContextSource::World)) {
		TArray<FHitResult> Hits;

		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (!IsValid(LocalPlayer) || !LocalPlayer->ViewportClient) return;

		FVector2D MousePos;
		if (!LocalPlayer->ViewportClient->GetMousePosition(MousePos)) return;
		
		// Convert mouse position to 3D raycast start and end points
		FVector WorldLocation, WorldDirection;
		PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("")), true, PlayerController);

		FVector Start = WorldLocation;
		FVector End = Start + WorldDirection * 10000.0f;
		
		if (PlayerController->GetWorld()->LineTraceMultiByChannel(Hits, Start, End, ECC_Visibility, TraceParams)) {

			TArray<FContextEntryPackage> ContextPackage;
			TArray<AActor*> HitActors;
			FVector FirstImpactPoint = Hits[0].ImpactPoint;

			// only get actors once
			for (auto Hit : Hits) {
				HitActors.AddUnique(Hit.GetActor());
			}

			// cleaned actors
			for (auto HitActor : HitActors) {

				UObject* ContextObject = ActionSubsystem->RetrieveValidContextHolderFromObjectNonConst(HitActor);
				if (!IsValid(ContextObject)) continue;
				
				// get entries + default
				TSet<UContext_ActionEntry*> ActionEntries = ActionSubsystem->GetValidContextEntriesForObject(ContextObject);
				ActionEntries.Append(DefaultActions);

				FContextEntryPackage EntryPackage(ContextObject, ActionEntries);
				ContextPackage.Add(EntryPackage);
				
			}

			if (ContextPackage.Num() != 0) {
				ActionSubsystem->ShowContextMenu(ContextPackage, FirstImpactPoint);
			}
		}
		return;
	}

	
}

APlayerController* UContext_SystemComponent::GetCurrentActorController() const {
	if (!IsValid(ActionSubsystem)) {
		UE_LOG(LogContextSystem, Log, TEXT("Oops! Looks like you're trying to open the context menu from an invalid context.\n"
											"ActionSubsystem is invalid in this context (Setup didn't run). Ensure you're calling this on the local player"));
		return nullptr;
	}

	const ACharacter* Player = Cast<ACharacter>(GetOwner());
	if (!ensure(Player)) {
		UE_LOG(LogContextSystem, Log, TEXT("Context_SystemComponent can only be placed on a player character!"));
		return nullptr;
	}
	APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
	if (!IsValid(PlayerController)) {
		return nullptr;
	}

	return PlayerController;
}
