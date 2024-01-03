// Copyright Epic Games, Inc. All Rights Reserved.

#include "NGEjercicioTecnicoProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ANGEjercicioTecnicoProjectile::ANGEjercicioTecnicoProjectile() 
{
	bReplicates = true;
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	if (GetLocalRole() == ROLE_Authority)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &ANGEjercicioTecnicoProjectile::OnHit);		// set up a notification for when this component hits something blocking
	}
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	ProjectileMovement->ProjectileGravityScale = 0.5f;

	// Die after 2 seconds by default
	InitialLifeSpan = 2.0f;
}

void ANGEjercicioTecnicoProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 50.0f, GetActorLocation());
	}
	SetLifeSpan(0.2f);
	
}