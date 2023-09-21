// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoMainMenu.h"

#include "DemoCharacter.h"
#include "DemoUIAmmoCount.h"
#include "Components/Button.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "DemoUIHud.h"
#include "DemoUIPoints.h"
#include "DemoUIAmmoCount.h"

void UDemoMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UDemoMainMenu::Initialize()
{
	Super::Initialize();

	if (StartButton)
		StartButton->OnClicked.AddUniqueDynamic(this, &UDemoMainMenu::StartGame);
	if (EndButton)
		EndButton->OnClicked.AddUniqueDynamic(this, &UDemoMainMenu::QuitGame);

	return true;
}

void UDemoMainMenu::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s]"), *FString(__FUNCTION__));

	// Remove MainMenu
	RemoveFromParent();

	// Shut Mouse Cursor
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	check(PlayerController);
	PlayerController->SetShowMouseCursor(false);

	// Get PlayerStart Transform
	TArray<AActor*> FoundActors;
	TSubclassOf<AActor> PlayerStartClass = APlayerStart::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerStartClass, FoundActors);
	check(FoundActors[0]);
	APlayerStart* PlayerStart = Cast<APlayerStart>(FoundActors[0]);
	FTransform PlayerStartTransform = PlayerStart->GetTransform();

	if (FirstPersonBP)
	{
		// Spawn FirstPerson
		FirstPerson = GetWorld()->SpawnActor<ADemoCharacter>(FirstPersonBP, PlayerStartTransform);

		// Camera Switch
		PlayerController->SetViewTargetWithBlend(FirstPerson, 1.0f);

		// Delay 1s for Camera Switch
		FTimerHandle MyTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &UDemoMainMenu::AfterDelay, 1.0f, false);
		UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] Delay Begin"), *FString(__FUNCTION__));
	}
}

void UDemoMainMenu::AfterDelay()
{
	UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] Delay Complete"), *FString(__FUNCTION__));

	// Possess FirstPerson
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	check(PlayerController);
	PlayerController->Possess(FirstPerson);
	UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] Posses Complete"), *FString(__FUNCTION__));

	// Could be implemented in BP, called by cpp
	// AfterPosses();

	// Create UI Widget: Hud & Points
	if (UIHudBP)
	{
		UDemoUIHud* UIHud = CreateWidget<UDemoUIHud>(GetWorld(), UIHudBP);
		UIHud->AddToViewport();
	}
	if (UIPointsBP)
	{
		UDemoUIPoints* UIPoints = CreateWidget<UDemoUIPoints>(GetWorld(), UIPointsBP);
		UIPoints->AddToViewport();
	}
	if (UIAmmoBP)
	{
		UDemoUIAmmoCount* UIAmmo = CreateWidget<UDemoUIAmmoCount>(GetWorld(), UIAmmoBP);
		UIAmmo->AddToViewport();
	}
}

void UDemoMainMenu::QuitGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s]"), *FString(__FUNCTION__));

	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit,
	                               false);
}
