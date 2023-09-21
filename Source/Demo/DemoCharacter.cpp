// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoCharacter.h"
#include "DemoProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
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

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ADemoCharacter::OnRifleOverlap);
}

//////////////////////////////////////////////////////////////////////////// Input

void ADemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
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
	AmmoComponent = OtherComp;
}

void ADemoCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void ADemoCharacter::DamageHealth()
{
	Health -= 0.25f;
}

void ADemoCharacter::ConsumeEnergy()
{
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
	if (AmmoComponent)
	{
		FVector LocationStart = AmmoComponent->GetComponentLocation();
		FVector LocationEnd = LocationStart + AmmoComponent->GetForwardVector() * 5000;
		FHitResult OutHit;
		UWorld* World = AmmoComponent->GetWorld();
		ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
		UKismetSystemLibrary::LineTraceSingle(World, LocationStart,
		                                      LocationEnd, TraceChannel, false, TArray<AActor*>(),
		                                      EDrawDebugTrace::Type::ForDuration, OutHit, true, FLinearColor::Red,
		                                      FLinearColor::Green, 0.1f);
		UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] Line Trace"), *FString(__FUNCTION__));
	}

	UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] AmmoComponent:%p"), *FString(__FUNCTION__), AmmoComponent);
}
