// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_PickUpComponent.h"

#include "DemoUIAmmoCount.h"
#include "DemoResetScene.h"
#include "DemoResetPoints.h"

UTP_PickUpComponent::UTP_PickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UTP_PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UTP_PickUpComponent::OnSphereBeginOverlap);
}

void UTP_PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                               const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	ADemoCharacter* Character = Cast<ADemoCharacter>(OtherActor);

	if (UIAmmoBP)
	{
		UDemoUIAmmoCount* UIAmmo = CreateWidget<UDemoUIAmmoCount>(GetWorld(), UIAmmoBP);
		UIAmmo->AddToViewport();
	}

	if (UIResetSceneBP)
	{
		UDemoResetScene* UIResetScene = CreateWidget<UDemoResetScene>(GetWorld(), UIResetSceneBP);
		UIResetScene->AddToViewport();
	}

	if (UIResetPointsBP)
	{
		UDemoResetPoints* UIResetPoints = CreateWidget<UDemoResetPoints>(GetWorld(), UIResetPointsBP);
		UIResetPoints->AddToViewport();
	}

	if (Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
