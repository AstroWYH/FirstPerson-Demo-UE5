// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoUIAmmoCount.h"

#include "DemoCharacter.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UDemoUIAmmoCount::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	check(PlayerCharacter);
	FirstPerson = Cast<ADemoCharacter>(PlayerCharacter);
	check(FirstPerson);
}

bool UDemoUIAmmoCount::Initialize()
{
	Super::Initialize();

	if (AmmoCountText)
		AmmoCountText->TextDelegate.BindUFunction(this, FName("GetAmmoCount"));
	if (MaxAmmoCountText)
		MaxAmmoCountText->TextDelegate.BindUFunction(this, FName("GetMaxAmmoCount"));

	return true;
}

FText UDemoUIAmmoCount::GetAmmoCount()
{
	return FText::FromString(FString::FromInt(FirstPerson->GetAmmoCount()));
}

FText UDemoUIAmmoCount::GetMaxAmmoCount()
{
	return FText::FromString(FString::FromInt(FirstPerson->GetMaxAmmoCount()));
}
