// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoCharacter.h"
#include "DemoProjectile.h"
#include "DemoTarget.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
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
	JumpActionDelegate.AddDynamic(this, &ADemoCharacter::ConsumeEnergy);
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

	// Damage Health
	PlayerInputComponent->BindAction("F", IE_Pressed, this, &ADemoCharacter::DamageHealth);

	// Reset Ammo
	PlayerInputComponent->BindAction("R", IE_Pressed, this, &ADemoCharacter::ResetAmmo);

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

	// movement speed switch
	PlayerInputComponent->BindAction("MovementSpeedSwitch", IE_Pressed, this, &ADemoCharacter::AccSpeed);
	PlayerInputComponent->BindAction("MovementSpeedSwitch", IE_Released, this, &ADemoCharacter::ResetSpeed);
}

void ADemoCharacter::OnRifleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	// For enable line trace
	bIsPickUp = true;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), DemoTargetBP, "DefaultTarget", FoundActors);
	check(FoundActors[0]);
	AActor* DefaultTarget = FoundActors[0];
	FTransform DefaultTranslation = DefaultTarget->GetTransform();
	FVector DefaultLocation = DefaultTranslation.GetLocation();
	FRotator DefaultRotation = DefaultTranslation.GetRotation().Rotator();
	FVector DefaultScale = DefaultTranslation.GetScale3D();

	FVector ALocation = DefaultLocation - FVector();

	ADemoTarget* SpawnTargetA = GetWorld()->SpawnActor<ADemoTarget>(DemoTargetBP, DefaultTarget->GetTransform());
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

void ADemoCharacter::DamageHealth()
{
	Health -= 0.25f;
}

void ADemoCharacter::ConsumeEnergy()
{
	UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s]"), *FString(__FUNCTION__));
	Energy -= 0.25f;
}

void ADemoCharacter::ResetAmmo()
{
	AmmoCount = MaxAmmoCount;
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
		// UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] Line Trace"), *FString(__FUNCTION__));
	}
}
