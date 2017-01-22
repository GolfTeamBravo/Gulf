// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Gulf.h"
#include "GulfBall.h"
//#include <Engine.h>

const float maxDeployPower = 3.0f;

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
	SpringArm->TargetArmLength = 1000.f;
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
    bIsDeploying = false;
    deployPower = 0.0f;
}


void AGulfBall::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGulfBall::Jump);
	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AGulfBall::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AGulfBall::TouchStopped);

    PlayerInputComponent->BindAxis("TurnCameraX", this, &AGulfBall::TurnCameraX);
    PlayerInputComponent->BindAxis("TurnCameraY", this, &AGulfBall::TurnCameraY);
    PlayerInputComponent->BindAxis("Zoom", this, &AGulfBall::Zoom);
    PlayerInputComponent->BindAction("Deploy", IE_Pressed, this, &AGulfBall::DeployStart);
    PlayerInputComponent->BindAction("Deploy", IE_Released, this, &AGulfBall::DeployEnd);
}

ENGINE_API void DrawDebugLine(
    const UWorld* InWorld,
    FVector const& LineStart,
    FVector const& LineEnd,
    FColor const& Color,
    bool bPersistentLines = false,
    float LifeTime = -1.f,
    uint8 DepthPriority = 0,
    float Thickness = 0.f
);

void AGulfBall::Tick(float delta) {
    APawn::Tick(delta);
    if (bIsDeploying) {
        auto direction = Camera->GetComponentRotation().Vector();
        direction.Z = 0.0f;
        auto color = FColor(deployPower / maxDeployPower * 255, 0, 128 - (deployPower / maxDeployPower) * 128, 255);
        DrawDebugLine(this->GetWorld(), Ball->GetComponentLocation(), Ball->GetComponentLocation() + direction * deployPower * 300, color, false, 0.0f, 0, 8.0f);
    }
}

void AGulfBall::DeployStart_Implementation() {
    bIsDeploying = true;
    deployPower = 0.0f;
}

void AGulfBall::DeployEnd_Implementation() {
    bIsDeploying = false;
	auto direction = Camera->GetComponentRotation().Vector();
    direction.Z = 0.0f;
    Ball->AddImpulse(direction * 10000.0f * deployPower * 2);
}

void AGulfBall::TurnCameraX_Implementation(float val) {
    SpringArm->AddRelativeRotation(FQuat(0.0f, 0.0f, val / 100.0f, 1.0f));
}

void AGulfBall::TurnCameraY_Implementation(float val) {
    if (bIsDeploying) {
        deployPower += val / 100.0f;
        if (deployPower < 0.0f) deployPower = 0.0f;
        if (deployPower > maxDeployPower) deployPower = maxDeployPower;
        return;
    }
    SpringArm->AddLocalRotation(FQuat(0.0f, -val / 100.0f, 0.0f, 1.0f));
    auto rot = SpringArm->GetRelativeTransform().Rotator();
    if (rot.Pitch > -5.0f) rot.Pitch = -5.0f;
    if (rot.Pitch < -66.0f) rot.Pitch = -65.0f;
    SpringArm->SetRelativeRotation(rot.Quaternion());
}

void AGulfBall::Zoom(float val) {
    SpringArm->TargetArmLength += val * -100.0f;
    if (SpringArm->TargetArmLength < 400) SpringArm->TargetArmLength = 400;
    if (SpringArm->TargetArmLength > 2000) SpringArm->TargetArmLength = 2000;
}

void AGulfBall::Jump_Implementation()
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

#include "UnrealNetwork.h"

void AGulfBall::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AGulfBall, bCanJump);
	DOREPLIFETIME(AGulfBall, deployPower);
	DOREPLIFETIME(AGulfBall, bIsDeploying);
	DOREPLIFETIME(AGulfBall, SpringArm);
	DOREPLIFETIME(AGulfBall, Camera);
}

bool AGulfBall::DeployStart_Validate()
{
	return true;
}

bool AGulfBall::DeployEnd_Validate()
{
	return true;
}

bool AGulfBall::Jump_Validate()
{
	return true;
}

bool AGulfBall::TurnCameraX_Validate(float val)
{
	return true;
}

bool AGulfBall::TurnCameraY_Validate(float val)
{
	return true;
}