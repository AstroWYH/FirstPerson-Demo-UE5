// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoTarget.h"

#include "DemoCharacter.h"
#include "DemoProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADemoTarget::ADemoTarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	RootComponent = TargetMesh;
}

// Called when the game starts or when spawned
void ADemoTarget::BeginPlay()
{
	Super::BeginPlay();

	TargetMesh->OnComponentHit.AddDynamic(this, &ADemoTarget::OnHit);

	InitLocation = GetActorLocation();
}

// Called every frame
void ADemoTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActorHasTag("DefaultTarget"))
	{
		RunningTime += DeltaTime;
		float LocationX = InitLocation.X + A * UKismetMathLibrary::DegSin(B * RunningTime);
		SetActorLocation(FVector(LocationX, InitLocation.Y, InitLocation.Z));
	}
}

void ADemoTarget::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	ADemoProjectile* Projectile = Cast<ADemoProjectile>(OtherActor);
	if (Projectile)
	{
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		check(PlayerCharacter);
		ADemoCharacter* FirstPerson = Cast<ADemoCharacter>(PlayerCharacter);
		check(FirstPerson);

		int Points = FirstPerson->GetPoints();
		Points++;
		FirstPerson->SetPoints(Points);

		UE_LOG(LogTemp, Warning, TEXT("[wyh] [%s] Points:%d"), *FString(__FUNCTION__),
		       FirstPerson->GetPoints());
	}
}
