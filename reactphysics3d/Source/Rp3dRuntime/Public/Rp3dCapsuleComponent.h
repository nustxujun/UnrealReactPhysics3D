#pragma once 

#include "CoreMinimal.h"
#include "Rp3dShapeComponent.h"
#include "Rp3dCapsuleComponent.generated.h"


UCLASS(ClassGroup = "reactphysics3d", meta = (DisplayName = "reactphysics3d Capsule Collision", BlueprintSpawnableComponent))
class RP3DRUNTIME_API URp3dCapsuleComponent : public URp3dShapeComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = "reactphysics3d", meta = (ClampMin = "0", UIMin = "0"))
		float CapsuleHalfHeight = 44.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = "reactphysics3d", meta = (ClampMin = "0", UIMin = "0"))
		float CapsuleRadius = 22.0f;

public:
	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
	float GetHalfHeight()const{return CapsuleHalfHeight;}
	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
	float GetRadius()const{return CapsuleRadius;}


	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual bool IsZeroExtent() const override;
	virtual struct FCollisionShape GetCollisionShape(float Inflation = 0.0f) const override;
	virtual bool AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const override;

	virtual void UpdateCollisionShape()override;
};