// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoCharacter.h"
#include "DemoProjectile.h"
#include "DemoTarget.h"
#include "TP_PickUpComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

//////////////////////////////////////////////////////////////////////////
// ADemoCharacter

ADemoCharacter::ADemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// set jump max count
	JumpMaxCount = 2;
}

void ADemoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Bind OnRifleOverlap
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ADemoCharacter::OnRifleOverlap);

	// Reserve Scene Init State
	TArray<AActor*> FoundActors;
	TSubclassOf<AActor> BluBoxClass = AStaticMeshActor::StaticClass();
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), BluBoxClass, "BlueBox", FoundActors);

	for (AActor* BlueBox : FoundActors)
	{
		SceneInitState.Push(BlueBox->GetTransform());
	}
	SceneInitState.Push(GetTransform());

	for (auto Val : SceneInitState)
	{
		UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] SceneInitState.Num():%d Val:%s"),
		       *FString(__FUNCTION__), SceneInitState.Num(), *Val.ToString());
	}

	JumpActionDelegate.AddDynamic(this, &ACharacter::Jump);
	JumpActionDelegate.AddDynamic(this, &ADemoCharacter::ConsumeEnergyForJump);
}

//////////////////////////////////////////////////////////////////////////// Input

void ADemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADemoCharacter::OnJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ADemoCharacter::OnPrimaryAction);

	// Reset State
	PlayerInputComponent->BindAction("R", IE_Pressed, this, &ADemoCharacter::ResetState);

	// AccSpeed
	PlayerInputComponent->BindAction("MovementSpeedSwitch", IE_Pressed, this, &ADemoCharacter::AccSpeed);
	PlayerInputComponent->BindAction("MovementSpeedSwitch", IE_Released, this, &ADemoCharacter::ResetSpeed);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ADemoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ADemoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ADemoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ADemoCharacter::LookUpAtRate);
}

void ADemoCharacter::OnRifleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	UTP_PickUpComponent* PickUpComponent = Cast<UTP_PickUpComponent>(OtherComp);
	if (PickUpComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] UTP_PickUpComponent OverLap"), *FString(__FUNCTION__));
		bIsPickUp = true;

		// /** -------------------------------------- Test Code --------------------------------------**/
		// if (DemoRifleBP)
		// {
		// 	FString ClassName = DemoRifleBP->GetDefaultObject()->GetClass()->GetName();
		// 	// ClassName: BP_Rifle_C OtherActor: BP_RiflePickUp_C_1
		// 	UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] ClassName: %s OtherActor: %s"), *FString(__FUNCTION__),
		// 	       *ClassName, *OtherActor->GetName());
		// }

		// Get Default Target Info
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), DemoTargetBP, "DefaultTarget", FoundActors);
		check(FoundActors[0]);
		AActor* DefaultTarget = FoundActors[0];
		UStaticMeshComponent* DefalutTargetMesh = DefaultTarget->FindComponentByClass<UStaticMeshComponent>();
		UMaterialInterface* DefalutTargetMaterial = LoadObject<UMaterialInterface>(
			nullptr, TEXT("Material'/Game/StarterContent/Props/Materials/M_Lamp.M_Lamp'"));
		DefalutTargetMesh->SetMaterial(1, DefalutTargetMaterial);

		FTransform DefaultTranslation = DefaultTarget->GetTransform();
		FVector DefaultLocation = DefaultTranslation.GetLocation();
		FRotator DefaultRotation = DefaultTranslation.GetRotation().Rotator();
		FVector DefaultScale = DefaultTranslation.GetScale3D();

		// UE_LOG(LogTemp, Warning,
		//        TEXT(
		// 	       "[wyh] [%s] DefaultTranslation:%s DefaultLocation:%s DefaultRotation:%s DefaultScale:%s DefaultLocation.Z:%f"
		//        ),
		//        *FString(__FUNCTION__),
		//        *DefaultTranslation.ToString(), *DefaultLocation.ToString(), *DefaultRotation.ToString(),
		//        *DefaultScale.ToString(), DefaultLocation.Z);

		// Spawn TargetA
		TargetA = GetWorld()->SpawnActor<ADemoTarget>(
			DemoTargetBP,
			FTransform(FRotator(DefaultRotation.Pitch, DefaultRotation.Yaw + 45.0f, DefaultRotation.Roll),
			           FVector(DefaultLocation.X - 200.0f, DefaultLocation.Y + 400.0f, DefaultLocation.Z),
			           DefaultScale));

		FTransform TargetATranslation = TargetA->GetTransform();
		TargetALocation = TargetATranslation.GetLocation();
		TargetARotation = TargetATranslation.GetRotation().Rotator();
		TargetAScale = TargetATranslation.GetScale3D();

		// USceneComponent* const SceneRootComponent = TargetA->GetRootComponent();
		// UE_LOG(LogTemp, Warning,
		//        TEXT(
		// 	       "[wyh] [%s] TargetATranslation:%s TargetALocation:%s TargetARotation:%s TargetAScale:%s TargetALocation.Z:%f SceneRootComponent:%s"
		//        ),
		//        *FString(__FUNCTION__),
		//        *TargetATranslation.ToString(), *TargetALocation.ToString(), *TargetARotation.ToString(),
		//        *TargetAScale.ToString(), TargetALocation.Z, *SceneRootComponent->GetName());

		UStaticMeshComponent* TargetAMesh = TargetA->FindComponentByClass<UStaticMeshComponent>();
		UMaterialInterface* TargetAMaterial = LoadObject<UMaterialInterface>(
			nullptr, TEXT("Material'/Game/StarterContent/Materials/M_Tech_Hex_Tile_Pulse.M_Tech_Hex_Tile_Pulse'"));
		TargetAMesh->SetMaterial(1, TargetAMaterial);

		// Spawn TargetB
		TargetB = GetWorld()->SpawnActor<ADemoTarget>(
			DemoTargetBP,
			FTransform(FRotator(DefaultRotation.Pitch, DefaultRotation.Yaw + 90.0f, DefaultRotation.Roll),
			           FVector(DefaultLocation.X + 200.0f, DefaultLocation.Y - 400.0f, DefaultLocation.Z),
			           DefaultScale));

		FTransform TargetBTranslation = TargetB->GetTransform();
		TargetBLocation = TargetBTranslation.GetLocation();
		TargetBRotation = TargetBTranslation.GetRotation().Rotator();
		TargetBScale = TargetBTranslation.GetScale3D();

		UStaticMeshComponent* TargetBMesh = TargetB->FindComponentByClass<UStaticMeshComponent>();
		UMaterialInterface* TargetBMaterial = LoadObject<UMaterialInterface>(
			nullptr, TEXT("Material'/Game/StarterContent/Materials/M_ColorGrid_LowSpec.M_ColorGrid_LowSpec'"));
		TargetBMesh->SetMaterial(1, TargetBMaterial);
	}
}

void ADemoCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void ADemoCharacter::OnJump()
{
	UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s]"), *FString(__FUNCTION__));
	JumpActionDelegate.Broadcast();
}

void ADemoCharacter::SetHealth(float NewHealth)
{
	Health = NewHealth;
}

void ADemoCharacter::SetEnergy(float NewEnergy)
{
	Energy = NewEnergy;
}

void ADemoCharacter::ConsumeEnergyForJump()
{
	Energy -= 0.1f;
}

void ADemoCharacter::ResetState()
{
	AmmoCount = MaxAmmoCount;
	Health = 1.0f;
	Energy = 1.0f;
}

void ADemoCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADemoCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADemoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ADemoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ADemoCharacter::AccSpeed()
{
	UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] AccSpeed"), *FString(__FUNCTION__));
	UCharacterMovementComponent* CharacterMovementPtr = GetCharacterMovement();

	if (CharacterMovementPtr)
	{
		CharacterMovementPtr->MaxWalkSpeed = 2000.0f;
	}
}

void ADemoCharacter::ResetSpeed()
{
	UCharacterMovementComponent* CharacterMovementPtr = GetCharacterMovement();

	if (CharacterMovementPtr)
	{
		CharacterMovementPtr->MaxWalkSpeed = 600.0f;
	}
}

void ADemoCharacter::Tick(float DeltaSeconds)
{
	RunningTime += DeltaSeconds;

	if (GetCharacterMovement()->MaxWalkSpeed > 1000.0f)
	{
		Energy -= 0.001f;
	}

	if (bIsPickUp)
	{
		FVector ComponentLocation = FirstPersonCameraComponent->GetComponentLocation();
		FVector LocationStart = ComponentLocation;
		FVector LocationEnd = ComponentLocation + FirstPersonCameraComponent->GetForwardVector() * 5000;
		FHitResult OutHit;
		ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), LocationStart,
		                                      LocationEnd, TraceChannel, false, TArray<AActor*>(),
		                                      EDrawDebugTrace::Type::ForOneFrame, OutHit, true, FLinearColor::Red,
		                                      FLinearColor::Green, 0.5f);

		// Set TargetA Location & Rotator
		FVector LocationA;
		LocationA.X = TargetRadius * UKismetMathLibrary::DegCos(TargetFreq * RunningTime) + TargetALocation.X;
		LocationA.Y = -1 * TargetRadius * UKismetMathLibrary::DegSin(TargetFreq * RunningTime) + TargetALocation.Y;
		LocationA.Z = TargetALocation.Z;
		TargetA->SetActorLocation(LocationA);
		FRotator RotatorA;
		RotatorA = TargetARotation;
		RotatorA.Yaw = TargetFreq * RunningTime + TargetARotation.Yaw;
		TargetA->SetActorRotation(RotatorA);

		// Set TargetB Location & Rotator
		FVector LocationB;
		LocationB.X = TargetRadius * UKismetMathLibrary::DegCos(TargetFreq * RunningTime) + TargetBLocation.X;
		LocationB.Y = -1 * TargetRadius * UKismetMathLibrary::DegSin(TargetFreq * RunningTime) + TargetBLocation.Y;
		LocationB.Z = TargetBLocation.Z;
		TargetB->SetActorLocation(LocationB);
		FRotator RotatorB;
		RotatorB = TargetBRotation;
		RotatorB.Yaw = TargetFreq * RunningTime + TargetBRotation.Yaw;
		TargetB->SetActorRotation(RotatorB);
	}
}
