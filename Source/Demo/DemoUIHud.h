// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoUIHud.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UDemoUIHud : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Demo")
	class ADemoCharacter* FirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UProgressBar* EnergyBar;

	UFUNCTION()
	float GetHealth();

	UFUNCTION()
	float GetEnergy();

protected:
	virtual void NativeConstruct() override;

	virtual bool Initialize() override;
};
