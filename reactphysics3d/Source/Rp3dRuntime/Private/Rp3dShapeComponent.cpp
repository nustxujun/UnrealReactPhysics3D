#include "Rp3dShapeComponent.h"

#include "Rp3dSystem.h"
#include "Rp3dWorld.h"
#include "Rp3dUtils.h"

#include "reactphysics3d/engine/PhysicsWorld.h"
#include "reactphysics3d/engine/PhysicsCommon.h"

URp3dShapeComponent::URp3dShapeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseEditorCompositing = true;
}



void URp3dShapeComponent::OnCreateRp3dState(TSharedPtr<FRp3dPhysicsWorld> RWorld)
{
	Super::OnCreateRp3dState(RWorld);

	if (!RigidBody )
		return;

	UpdateCollisionShape();
	const auto Num = CollisionShapes.Num();
	for (int i = 0; i < Num; ++i)
	{
		auto Collider = RigidBody->AddCollisionShape(CollisionShapes[i], UE_TO_RP3D(ShapeTransforms[i]));
		Collider.SetBounciness(Bounciness);
		Colliders.Add(Collider);
	}
	RigidBody->UpdateMassPropertiesFromColliders();
}

void URp3dShapeComponent::AddCollisionShape(Rp3dCollisionShape::Ptr Shape, const FTransform& Transform)
{
	CollisionShapes.Add(Shape);
	ShapeTransforms.Add(Transform);
}

void URp3dShapeComponent::ClearShapes()
{
	if (RigidBody)
		RigidBody->RemoveAllCollisionShapes();
	Colliders.Reset();
	CollisionShapes.Reset();
}

void URp3dShapeComponent::BeginDestroy()
{
	Super::BeginDestroy();
	ClearShapes();
}

void URp3dShapeComponent::SetBounciness(float Val)
{
	for (auto Collider : Colliders)
	{
		Collider.SetBounciness(Val);
	}
}