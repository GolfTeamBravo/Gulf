// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Pawn.h"
#include "GulfBall.generated.h"

UCLASS(config=Game)
class AGulfBall : public APawn
{
	GENERATED_BODY()

	/** StaticMesh used for the ball */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Ball;

	/** Spring arm for positioning the camera above the ball */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	/** Camera to view the ball */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

public:
	AGulfBall();

    virtual void Tick(float delta) override;

	/** Vertical impulse to apply when pressing jump */
	UPROPERTY(EditAnywhere, Category=Ball)
	float JumpImpulse;

	/** Torque to apply when trying to roll ball */
	UPROPERTY(EditAnywhere, Category=Ball)
	float RollTorque;

	/** Indicates whether we can currently jump, use to prevent double jumping */
	UPROPERTY(Replicated)
	bool bCanJump;

	UPROPERTY(Replicated)
    bool bIsDeploying;

	UPROPERTY(Replicated)
    float deployPower;
protected:
	UFUNCTION(Reliable, Server, WithValidation)
    void TurnCameraX(float val);
	virtual bool TurnCameraX_Validate();
	UFUNCTION(Reliable, Server, WithValidation)
    void TurnCameraY(float val);
	virtual bool TurnCameraY_Validate();
    void Zoom(float val);

	UFUNCTION(Reliable, Server, WithValidation)
    void DeployStart();
	virtual bool DeployStart_Validate();
	UFUNCTION(Reliable, Server, WithValidation)
    void DeployEnd();
	virtual bool DeployEnd_Validate();

	/** Handle jump action. */
	UFUNCTION(Reliable, Server, WithValidation)
	void Jump();
	virtual bool Jump_Validate();

	// AActor interface
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End of AActor interface

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

public:
	/** Returns Ball subobject **/
	FORCEINLINE class UStaticMeshComponent* GetBall() const { return Ball; }
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
};
