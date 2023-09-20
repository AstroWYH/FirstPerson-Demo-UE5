// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoUIPoints.generated.h"

UCLASS()
class DEMO_API UDemoUIPoints : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Demo")
	class ADemoCharacter* FirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* PointsText;

	UFUNCTION()
	FText GetPoints();
	
protected:
	virtual void NativeConstruct() override;

	virtual bool Initialize() override;

	// virtual void BeginDestroy() override;
};
