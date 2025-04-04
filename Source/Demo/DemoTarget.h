// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoTarget.generated.h"

UCLASS()
class DEMO_API ADemoTarget : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADemoTarget();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* TargetMesh;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Demo)
	int A = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Demo)
	int B = 100;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	float RunningTime = 0.0f;

	FVector InitLocation;
};
