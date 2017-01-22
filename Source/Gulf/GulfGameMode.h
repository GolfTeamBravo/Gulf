// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "GulfGameMode.generated.h"

UCLASS(minimalapi)
class AGulfGameMode : public AGameModeBase
{
	GENERATED_BODY()

	int numInHole;
	int holeNumber;
	TArray<FTransform> spawns;
public:
	UFUNCTION(BlueprintCallable, Category="Gulf")
		void BallInHole();

	UFUNCTION(BlueprintCallable, Category="Gulf")
	void RegisterHoleSpawn(FTransform trans);

	AGulfGameMode();
};