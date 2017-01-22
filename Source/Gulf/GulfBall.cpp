// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Gulf.h"
#include "GulfBall.h"

AGulfBall::AGulfBall()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallMesh(TEXT("/Game/Rolling/Meshes/BallMesh.BallMesh"));

	// Create mesh component for the ball
	Ball = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball0"));
    Ball->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));
	Ball->SetStaticMesh(BallMesh.Object);
	Ball->BodyInstance.SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Ball->SetSimulatePhysics(true);
	Ball->SetAngularDamping(0.1f);
	Ball->SetLinearDamping(0.1f);
	Ball->BodyInstance.MassScale = 1.0f;
	Ball->BodyInstance.MaxAngularVelocity = 100.0f;
	Ball->SetNotifyRigidBodyCollision(true);
	RootComponent = Ball;

	// Create a camera boom attached to the root (ball)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bAbsoluteRotation = true; // Rotation of the ball should not affect rotation of boom
	SpringArm->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	SpringArm->TargetArmLength = 1200.f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 3.f;

	// Create a camera and attach to boom
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Set up forces
	RollTorque = 50000000.0f;
	JumpImpulse = 3500.0f;
	bCanJump = true; // Start being able to jump
}


void AGulfBall::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAxis("MoveRight", this, &AGulfBall::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGulfBall::MoveForward);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGulfBall::Jump);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AGulfBall::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AGulfBall::TouchStopped);

    PlayerInputComponent->BindAxis("TurnCameraX", this, &AGulfBall::TurnCameraX);
    PlayerInputComponent->BindAxis("TurnCameraY", this, &AGulfBall::TurnCameraY);
}

void AGulfBall::TurnCameraX(float val) {
    Camera->AddLocalRotation(FQuat(0.0f, 0.0f, val / 100.0f, 1.0f));
}

void AGulfBall::TurnCameraY(float val) {
    Camera->AddLocalRotation(FQuat(0.0f, -val / 100.0f, 0.0f, 1.0f));
}

void AGulfBall::MoveRight(float Val)
{
	const FVector Torque = FVector(-1.f * Val * RollTorque, 0.f, 0.f);
	Ball->AddTorque(Torque);
}

void AGulfBall::MoveForward(float Val)
{
	const FVector Torque = FVector(0.f, Val * RollTorque, 0.f);
	Ball->AddTorque(Torque);	
}

void AGulfBall::Jump()
{
	if(bCanJump)
	{
		const FVector Impulse = FVector(0.f, 0.f, JumpImpulse);
		Ball->AddImpulse(Impulse);
		bCanJump = false;
	}
}

void AGulfBall::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	bCanJump = true;
}

void AGulfBall::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bCanJump)
	{
		const FVector Impulse = FVector(0.f, 0.f, JumpImpulse);
		Ball->AddImpulse(Impulse);
		bCanJump = false;
	}

}

void AGulfBall::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bCanJump)
	{
		const FVector Impulse = FVector(0.f, 0.f, JumpImpulse);
		Ball->AddImpulse(Impulse);
		bCanJump = false;
	}
}
