#pragma once 

#include "CoreMinimal.h"
#include "Rp3dCommon.h"
#include "Rp3dRigidBody.h"
#include "Rp3dRigidBodyComponent.h"
#include "Rp3dShapeComponent.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RP3DRUNTIME_API URp3dShapeComponent : public URp3dRigidBodyComponent
{
	GENERATED_UCLASS_BODY()
public:
	virtual void OnCreateRp3dState(TSharedPtr<FRp3dPhysicsWorld> RWorld) override;
	virtual void UpdateCollisionShape() {};
	virtual void BeginDestroy()override ;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,export, Category = "reactphysics3d")
	float Bounciness = 0.0f;

protected:
	void AddCollisionShape(Rp3dCollisionShape::Ptr Shape, const FTransform& Transform);
	void ClearShapes();

public:
	void SetBounciness(float Val);

private:
	TArray<Rp3dCollisionShape::Ptr> CollisionShapes;
	TArray<FTransform> ShapeTransforms;
	TArray<FRp3dCollider> Colliders;
};