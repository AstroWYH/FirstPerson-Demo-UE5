// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoResetPoints.generated.h"

UCLASS()
class DEMO_API UDemoResetPoints : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Demo")
	class ADemoCharacter* FirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* ResetPointsButton;
	
	UFUNCTION()
	void ResetPoints();
	
protected:
	virtual void NativeConstruct() override;

	virtual bool Initialize() override;
};
