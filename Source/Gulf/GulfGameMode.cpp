// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Gulf.h"
#include "GulfGameMode.h"
#include "GulfBall.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

void AGulfGameMode::BallInHole()
{
	if (++numInHole >= GetNumPlayers())
	{
		numInHole = 0;
		++holeNumber;
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			FTransform &trans = spawns[holeNumber];
			Iterator->Get()->GetPawn()->TeleportTo(trans.GetTranslation(), trans.GetRotation().Rotator(), false, true);
			Iterator->Get()->GetPawn()->SetActorHiddenInGame(false);
		}
	}
}

void AGulfGameMode::RegisterHoleSpawn(FTransform trans)
{
	spawns.Add(trans);
	if (holeNumber == 0 && spawns.Num() == 1)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			Iterator->Get()->GetPawn()->SetActorEnableCollision(false);
			Iterator->Get()->GetPawn()->SetActorLocationAndRotation(trans.GetLocation(), trans.GetRotation().Rotator(), false, nullptr, ETeleportType::TeleportPhysics);
			Iterator->Get()->GetPawn()->SetActorEnableCollision(true);
		}
	}
}

AGulfGameMode::AGulfGameMode()
{
	// set default pawn class to our ball
	DefaultPawnClass = AGulfBall::StaticClass();
}