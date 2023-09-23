// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DemoCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpActionDelegate);

UCLASS(config=Game)
class ADemoCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADemoCharacter();

	virtual void Tick(float DeltaSeconds) override;

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FJumpActionDelegate JumpActionDelegate;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	UPrimitiveComponent* AmmoComponent;

	// getter and setter
	int GetPoints() { return Points; }

	void SetPoints(int NewPoints) { Points = NewPoints; }

	float GetHealth() { return Health; }

	void SetHealth(float NewHealth);

	float GetEnergy() { return Energy; }

	void SetEnergy(float NewEnergy);

	UFUNCTION()
	void ConsumeEnergyForJump();

	int GetAmmoCount() const { return AmmoCount; }

	void SetAmmoCount(int NewAmmoCount) { AmmoCount = NewAmmoCount; }

	int GetMaxAmmoCount() const { return MaxAmmoCount; }

	void GetMaxAmmoCount(int NewMaxAmmoCount) { AmmoCount = NewMaxAmmoCount; }

	UFUNCTION()
	void OnRifleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepResult);

	TArray<FTransform> SceneInitState;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADemoTarget> DemoTargetBP;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> DemoRifleBP;

protected:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Fires a projectile. */
	void OnPrimaryAction();

	void OnJump();

	void ResetState();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void AccSpeed();

	void ResetSpeed();

	virtual void BeginPlay();
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Points = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Health = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Energy = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int AmmoCount = 25;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int MaxAmmoCount = 25;

	bool bIsPickUp = false;

	ADemoTarget* TargetA;
	ADemoTarget* TargetB;

	int TargetFreq = 100;
	int TargetRadius = 500;
	float RunningTime = 0.0f;

	FVector TargetALocation;
	FRotator TargetARotation;
	FVector TargetAScale;

	FVector TargetBLocation;
	FRotator TargetBRotation;
	FVector TargetBScale;
};
