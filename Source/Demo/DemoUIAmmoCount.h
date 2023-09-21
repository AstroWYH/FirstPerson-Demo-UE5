// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoUIAmmoCount.generated.h"

UCLASS()
class DEMO_API UDemoUIAmmoCount : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Demo")
	class ADemoCharacter* FirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* AmmoCountText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* MaxAmmoCountText;

	UFUNCTION()
	FText GetAmmoCount();

	UFUNCTION()
	FText GetMaxAmmoCount();

protected:
	virtual void NativeConstruct() override;

	virtual bool Initialize() override;
};
