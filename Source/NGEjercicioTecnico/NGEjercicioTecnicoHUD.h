// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NGEjercicioTecnicoHUD.generated.h"

UCLASS()
class ANGEjercicioTecnicoHUD : public AHUD
{
	GENERATED_BODY()

public:
	ANGEjercicioTecnicoHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
protected:
	virtual void BeginPlay() override;
};

