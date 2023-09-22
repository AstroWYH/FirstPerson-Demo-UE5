// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoResetScene.generated.h"

UCLASS()
class DEMO_API UDemoResetScene : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Demo")
	class ADemoCharacter* FirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* ResetSceneButton;
	
	UFUNCTION()
	void ResetScene();
	
protected:
	virtual void NativeConstruct() override;

	virtual bool Initialize() override;
};
