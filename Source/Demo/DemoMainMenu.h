// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoMainMenu.generated.h"

UCLASS()
class DEMO_API UDemoMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Demo")
	class ADemoCharacter* FirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* StartButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* EndButton;

	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void QuitGame();

	UFUNCTION()
	void AfterDelay();

	// UFUNCTION(BlueprintImplementableEvent)
	// void AfterPosses();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADemoCharacter> FirstPersonBP;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDemoUIHud> UIHudBP;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDemoUIPoints> UIPointsBP;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDemoUIAmmoCount> UIAmmoBP;
	
protected:
	virtual void NativeConstruct() override;

	virtual bool Initialize() override;
};
