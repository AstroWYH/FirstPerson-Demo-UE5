// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoResetScene.h"

#include "DemoCharacter.h"
#include "Components/Button.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"


void UDemoResetScene::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UDemoResetScene::Initialize()
{
	Super::Initialize();

	if (ResetSceneButton)
		ResetSceneButton->OnClicked.AddUniqueDynamic(this, &UDemoResetScene::ResetScene);

	return true;
}

void UDemoResetScene::ResetScene()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	check(PlayerCharacter);
	FirstPerson = Cast<ADemoCharacter>(PlayerCharacter);
	check(FirstPerson);

	TArray<AActor*> FoundActors;
	TSubclassOf<AActor> BluBoxClass = AStaticMeshActor::StaticClass();
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), BluBoxClass, "BlueBox", FoundActors);


	for (int i = 0; i < FoundActors.Num(); i++)
	{
		FoundActors[i]->SetActorTransform(FirstPerson->SceneInitState[i]);
		UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] FoundActors.Num():%d FirstPerson->SceneInitState[%d]:%s"),
		       *FString(__FUNCTION__),
		       FoundActors.Num(), i, *FirstPerson->SceneInitState[i].ToString());
	}
	FirstPerson->SetActorTransform(FirstPerson->SceneInitState.Last());
}
