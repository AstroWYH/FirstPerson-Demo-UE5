// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoUIHud.h"

#include "DemoCharacter.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"


void UDemoUIHud::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	check(PlayerCharacter);
	FirstPerson = Cast<ADemoCharacter>(PlayerCharacter);
	check(FirstPerson);
}

bool UDemoUIHud::Initialize()
{
	Super::Initialize();

	if (HealthBar)
		HealthBar->PercentDelegate.BindUFunction(this, FName("GetHealth"));
	if (EnergyBar)
		EnergyBar->PercentDelegate.BindUFunction(this, FName("GetEnergy"));
	
	return true;
}

float UDemoUIHud::GetHealth()
{
	return FirstPerson->GetHealth();
}

float UDemoUIHud::GetEnergy()
{
	return FirstPerson->GetEnergy();
}
