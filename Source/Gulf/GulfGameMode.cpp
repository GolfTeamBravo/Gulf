// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Gulf.h"
#include "GulfGameMode.h"
#include "GulfBall.h"

AGulfGameMode::AGulfGameMode()
{
	// set default pawn class to our ball
	DefaultPawnClass = AGulfBall::StaticClass();
}
