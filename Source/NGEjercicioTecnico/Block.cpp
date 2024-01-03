#include "Block.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NGEjercicioTecnicoProjectile.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"

ABlock::ABlock()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	bNetUseOwnerRelevancy = 10;

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(VisualMesh);

	if (HasAuthority()) VisualMesh->OnComponentHit.AddDynamic(this, &ABlock::OnHit);
	VisualMesh->SetCollisionProfileName("Block");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube"));

	if (CubeVisualAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(CubeVisualAsset.Object);
		VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}
}

void ABlock::BeginPlay()
{
	Super::BeginPlay();

	int32 RandomNumber = FMath::RandRange(1, 3);

	if (HasAuthority())
	{
		switch (RandomNumber)
		{
		case 1:
			MaterialColor = FColor::FromHex(FString("FF001E"));
			CurrentColor = BlockColor::Red;
			break;
		case 2:
			MaterialColor = FColor::FromHex(FString("57FA00"));
			CurrentColor = BlockColor::Green;
			break;
		case 3:
			MaterialColor = FColor::FromHex(FString("0073E6"));
			CurrentColor = BlockColor::Blue;
			break;
		}
	}
	else 
	{
		SetActorTickEnabled(false);
	}
	
	Material = UMaterialInstanceDynamic::Create(VisualMesh->GetMaterial(0), NULL);
	Material->SetVectorParameterValue(FName(TEXT("Color")), MaterialColor);
	VisualMesh->SetMaterial(0, Material);

	DefaultCollisionSpeed = FVector(500, 0, 10) * 100;

}

void ABlock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlock, MaterialColor, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ABlock, CurrentColor, COND_InitialOnly);
	DOREPLIFETIME(ABlock, bHitted);
}

void ABlock::OnRep_Hitted()
{
	Hit();
}

void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector rt = LerpedPosition;
	
	if (FVector::Dist(NewPosition, rt) > 0.01f)
	{
		rt.X = FMath::Lerp<float, float>(rt.X, NewPosition.X, 0.04f);
		rt.Y = FMath::Lerp<float, float>(rt.Y, NewPosition.Y, 0.04f);
		rt.Z = FMath::Lerp<float, float>(rt.Z, NewPosition.Z, 0.04f);

		SetActorRelativeLocation(rt);
		LerpedPosition = rt;
	}
}

void ABlock::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& _Hit)
{
	if (HasAuthority())
	{
		if (OtherComp->GetCollisionProfileName() == FName("Projectile"))
		{
			ProjectileOwnerName = OtherActor->GetNetOwner()->GetActorLabel();

			Hit();
		}
	}
}

void ABlock::SetInitialPosition(FVector NewPos)
{
	SetActorRelativeLocation(NewPos);
	LerpedPosition = NewPos;
	NewPosition = NewPos;
	DefaultCollisionLocation = GetActorLocation();
	DefaultCollisionLocation.X -= 50;
	DefaultCollisionLocation.Y += 50;
	DefaultCollisionLocation.Z += 50;
}


void ABlock::Hit()
{
	bHitted = true;

	VisualMesh->SetSimulatePhysics(true);
	VisualMesh->SetEnableGravity(false);
	//VisualMesh->SetCollision;
	VisualMesh->AddImpulseAtLocation(DefaultCollisionSpeed, DefaultCollisionLocation);

	FTimerHandle TimeHandler = FTimerHandle();
	GetWorld()->GetTimerManager().SetTimer(TimeHandler, this, &ABlock::DelayedDestroy, 2.f, false);
}

void ABlock::DelayedDestroy()
{
	Destroy();
}

bool ABlock::WasHitted()
{
	return bHitted;
}

void ABlock::SetPosition(FVector NewPos)
{
	NewPosition = NewPos;
}

BlockColor ABlock::GetColor()
{
	return CurrentColor;
}

FString ABlock::GetProjectileOwnerLabel()
{
	return ProjectileOwnerName;
}



