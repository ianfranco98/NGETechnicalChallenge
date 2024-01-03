#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Block.generated.h"

UENUM()
enum class BlockColor : uint8
{
	Red,
	Green,
	Blue
};

UCLASS()
class NGEJERCICIOTECNICO_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlock();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	
	UPROPERTY(Replicated)
	FColor MaterialColor;
	UPROPERTY(Replicated)
	BlockColor CurrentColor;

	UPROPERTY(ReplicatedUsing = OnRep_Hitted)
	bool bHitted;

	FVector NewPosition;
	FVector LerpedPosition;
	FVector DefaultCollisionSpeed;
	FVector DefaultCollisionLocation;
	FString ProjectileOwnerName;

	UStaticMeshComponent* VisualMesh;
	UMaterialInstanceDynamic* Material;

	UFUNCTION()
	void OnRep_Hitted();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void DelayedDestroy();

public:
	void Hit();

	void SetInitialPosition(FVector NewPos);
	void SetPosition(FVector NewPos);
	
	FString GetProjectileOwnerLabel();
	BlockColor GetColor();
	bool WasHitted();
	
	
};
