#pragma once 

#include "CoreMinimal.h"
#include "Rp3dWorldSettings.generated.h"


USTRUCT(BlueprintType)
struct FRp3dWorldSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Common)
	/// Name of the world
	FString WorldName = TEXT("UnnamedWorld");

	UPROPERTY(EditAnywhere, Category = Common)
	/// Gravity force vector of the world (in Newtons)
	FVector Gravity = { 0, 0 ,-981 };

	UPROPERTY(EditAnywhere, Category = Simulation)
	/// Distance threshold for two contact points for a valid persistent contact (in cm)
	float PersistentContactDistanceThreshold = 3.0f;

	UPROPERTY(EditAnywhere, Category = Simulation)
	/// Default friction coefficient for a rigid body
	float DefaultFrictionCoefficient = 0.3f;

	UPROPERTY(EditAnywhere, Category = Simulation)
	/// Default bounciness factor for a rigid body
	float DefaultBounciness = 0.0f;

	UPROPERTY(EditAnywhere, Category = Simulation)
	/// Velocity threshold for contact velocity restitution (in cm)
	float RestitutionVelocityThreshold = 50.f;

	UPROPERTY(EditAnywhere, Category = Common)
	/// True if the sleeping technique is enabled
	bool bIsSleepingEnabled = false;

	UPROPERTY(EditAnywhere, Category = Simulation)
	/// Number of iterations when solving the velocity constraints of the Sequential Impulse technique
	uint16 DefaultVelocitySolverNbIterations = 6;

	UPROPERTY(EditAnywhere, Category = Simulation)
	/// Number of iterations when solving the position constraints of the Sequential Impulse technique
	uint16 DefaultPositionSolverNbIterations = 3;

	UPROPERTY(EditAnywhere, Category = Common)
	/// Time (in seconds) that a body must stay still to be considered sleeping
	float DefaultTimeBeforeSleep = 1.0f;

	UPROPERTY(EditAnywhere, Category = Common)
	/// A body with a linear velocity smaller than the sleep linear velocity (in cm/s)
	/// might enter sleeping mode.
	float DefaultSleepLinearVelocity = 2.0;

	UPROPERTY(EditAnywhere, Category = Common)
	/// A body with angular velocity smaller than the sleep angular velocity (in rad/s)
	/// might enter sleeping mode
	float DefaultSleepAngularVelocity = 3 * (PI / 180.0);

	UPROPERTY(EditAnywhere, Category = Simulation)
	/// This is used to test if two contact manifold are similar (same contact normal) in order to
	/// merge them. If the cosine of the angle between the normals of the two manifold are larger
	/// than the value bellow, the manifold are considered to be similar.
	float CosAngleSimilarContactManifold = 0.95f;


	UPROPERTY(EditAnywhere, Category = Common)
	bool bAutoUpdate = true;

};

