#pragma once 
#include "CoreMinimal.h"
#include "Rp3dCommon.h"
#include "Rp3dShapeComponent.h"
#include "Rp3dSphereComponent.generated.h"


UCLASS(ClassGroup="reactphysics3d", meta=(DisplayName="reactphysics3d Sphere Collision", BlueprintSpawnableComponent))
class RP3DRUNTIME_API URp3dSphereComponent : public URp3dShapeComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = "reactphysics3d")
	float SphereRadius = 32.0f;

public:

	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
	void SetSphereRadius(float InSphereRadius, bool bUpdateOverlaps = true);


	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool IsZeroExtent() const override;
	virtual struct FCollisionShape GetCollisionShape(float Inflation = 0.0f) const override;
	virtual bool AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual void CalcBoundingCylinder(float& CylinderRadius, float& CylinderHalfHeight) const override;
	//~ End USceneComponent Interface




	virtual void UpdateCollisionShape() override;

};