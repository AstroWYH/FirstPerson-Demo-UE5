// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoProjectile.h"

#include "DemoCharacter.h"
#include "DemoGameMode.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ADemoProjectile::ADemoProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ADemoProjectile::OnHit);
	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	UE_LOG(LogTemp, Warning, TEXT("wyh [%s] log test"), *FString(__FUNCTION__));

	// UWorld* world = GetWorld();
	// if (world)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("wyh [%s] world is not null"), *FString(__FUNCTION__));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("wyh [%s] world is null"), *FString(__FUNCTION__));
	// }
	// ADemoGameMode* gm = Cast<ADemoGameMode>(GetWorld()->GetAuthGameMode());
	// if (gm != nullptr)
	// {
	// 	gm->Points++;
	// 	UE_LOG(LogTemp, Warning, TEXT("wyh [%s] gm:%d"), *FString(__FUNCTION__), gm->Points);
	// }
}

void ADemoProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}