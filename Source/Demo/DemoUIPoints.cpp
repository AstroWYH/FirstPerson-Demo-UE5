// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoUIPoints.h"

#include "DemoCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

void UDemoUIPoints::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	check(PlayerCharacter);
	FirstPerson = Cast<ADemoCharacter>(PlayerCharacter);
	check(FirstPerson);
}

bool UDemoUIPoints::Initialize()
{
	Super::Initialize();

	PointsText->SetText(FText::FromString("Null"));

	// call every frame, can't be called in NativeConstruct()
	PointsText->TextDelegate.BindUFunction(this, FName("GetPoints"));

	return true;
}

// void UDemoUIPoints::BeginDestroy()
// {
// 	PointsText->TextDelegate.Unbind();
// 	Super::BeginDestroy();
// }

FText UDemoUIPoints::GetPoints()
{
	FString Points = FString::Printf(TEXT("%d"), FirstPerson->GetPoints());
	// UE_LOG(LogTemp, Warning, TEXT("wyh [%s] Points:%d"), *FString(__FUNCTION__), FirstPerson->GetPoints());
	return FText::FromString(Points);
}
