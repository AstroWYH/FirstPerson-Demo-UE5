// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoResetPoints.h"

#include "DemoCharacter.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UDemoResetPoints::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UDemoResetPoints::Initialize()
{
	Super::Initialize();

	if (ResetPointsButton)
		ResetPointsButton->OnClicked.AddUniqueDynamic(this, &UDemoResetPoints::ResetPoints);

	return true;
}

void UDemoResetPoints::ResetPoints()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	check(PlayerCharacter);
	FirstPerson = Cast<ADemoCharacter>(PlayerCharacter);
	check(FirstPerson);

	FirstPerson->SetPoints(0);
}
