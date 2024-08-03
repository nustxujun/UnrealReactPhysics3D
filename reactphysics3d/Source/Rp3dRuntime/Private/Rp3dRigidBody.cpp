#include "Rp3dRigidBody.h"
#include "Rp3dSystem.h"
#include "Rp3dWorld.h"
#include "Rp3dUtils.h"
#include "Rp3dCollisionShape.h"

#include "reactphysics3d/engine/PhysicsWorld.h"
#include "reactphysics3d/engine/PhysicsCommon.h"




void FRp3dCollider::SetBounciness(reactphysics3d::decimal Val)
{
    if (!IsValid())
        return;
    Collider->getMaterial().setBounciness(Val);
}

void FRp3dCollider::SetFriction(reactphysics3d::decimal Val)
{
    if (!IsValid())
        return;
    Collider->getMaterial().setFrictionCoefficient(Val);
}

void FRp3dCollider::SetDensity(reactphysics3d::decimal Val)
{
    if (!IsValid())
        return;
    Collider->getMaterial().setMassDensity(Val);
}

URp3dRigidBody::URp3dRigidBody(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URp3dRigidBody::Initialize(URp3dWorld* RWorld) 
{
    PhysicsWorld = RWorld;
    auto& Rp3dWorld = PhysicsWorld->GetRp3dPhysicsWorld();

    RigidBody = TSharedPtr<reactphysics3d::RigidBody>(Rp3dWorld.createRigidBody({UE_TO_RP3D(FTransform::Identity)}),
    [&Rp3dWorld](auto Body) {
            Rp3dWorld.destroyRigidBody(Body);
    });

    RWorld->AddRigidBody(this);
}

void URp3dRigidBody::SetBodyType(EBodyType Type)
{
	using namespace reactphysics3d;
	switch (Type)
	{
		case EBodyType::STATIC: RigidBody->setType(BodyType::STATIC); break;
		case EBodyType::KINEMATIC: RigidBody->setType(BodyType::KINEMATIC); break;
		case EBodyType::DYNAMIC: RigidBody->setType(BodyType::DYNAMIC); break;
	}
}

void URp3dRigidBody::SetIsDebugEnabled(bool Enable)
{
    RigidBody->setIsDebugEnabled(Enable);
}


void URp3dRigidBody::SetTransform(const Rp3dTransform& Trans)
{
    RigidBody->setTransform(Trans);
}

const Rp3dTransform& URp3dRigidBody::GetTransform()const
{
    return RigidBody->getTransform();
}

Rp3dVector3 URp3dRigidBody::GetLinearVelocity()const
{
    return RigidBody->getLinearVelocity();
}

void URp3dRigidBody::SetLinearVelocity(const Rp3dVector3& Vel)
{
    RigidBody->setLinearVelocity(Vel);
}

Rp3dVector3 URp3dRigidBody::GetAngularVelocity()const
{
    return RigidBody->getAngularVelocity();
}
void URp3dRigidBody::SetAngularVelocity(const Rp3dVector3& Vel)
{
    RigidBody->setAngularVelocity(Vel);
}




void URp3dRigidBody::SetAngularLockAxisFactor(const Rp3dVector3& Factor)
{
	RigidBody->setAngularLockAxisFactor(Factor);
}


void URp3dRigidBody::ApplyForceAtCenterOfMass(const Rp3dVector3& Force, bool bLocalSpace )
{
    if (bLocalSpace)
    {
        RigidBody->applyLocalForceAtCenterOfMass(Force);
    }
    else
    {
        RigidBody->applyWorldForceAtCenterOfMass(Force);
    }
}

void URp3dRigidBody::ApplyForceAtPoint(const Rp3dVector3& Force, const Rp3dVector3& Point, bool bLocalForce , bool bLocalPoint )
{
    if (bLocalForce)
    {
        if (bLocalPoint)
        {
            RigidBody->applyLocalForceAtLocalPosition(Force, Point);
        }
        else
        {
            RigidBody->applyLocalForceAtWorldPosition(Force, Point);
        }
    }
    else
    {
        if (bLocalPoint)
        {
            RigidBody->applyWorldForceAtLocalPosition(Force, Point);
        }
        else
        {
            RigidBody->applyWorldForceAtWorldPosition(Force, Point);
        }
    }
}

const Rp3dVector3& URp3dRigidBody::GetForce()const
{
    return RigidBody->getForce();
}

void URp3dRigidBody::ResetForce()
{
    RigidBody->resetForce();
}

void URp3dRigidBody::RemoveFromWorld()
{
    RigidBody.Reset();
    if (PhysicsWorld)
    {
        PhysicsWorld->RemoveRigidBody(this);
        PhysicsWorld = nullptr;
    }
    CollisionShapes.Reset();
}

void URp3dRigidBody::BeginDestroy()
{
    Super::BeginDestroy();
    RemoveFromWorld();
}

bool URp3dRigidBody::IsActive()const
{
    return RigidBody->isActive();
}

FRp3dCollider URp3dRigidBody::AddCollisionShape(Rp3dCollisionShape::Ptr Shape, const Rp3dTransform& Trans)
{
    auto Collider = RigidBody->addCollider(Shape->GetRp3dShape(), Trans);
    CollisionShapes.Add(Shape);
    FRp3dCollider RCollider;
    RCollider.Initialize(Collider, this);
    return RCollider;
}

void URp3dRigidBody::RemoveAllCollisionShapes()
{
    while (RigidBody && RigidBody->getNbColliders())
    {
        auto Co = RigidBody->getCollider(RigidBody->getNbColliders() - 1);
        RigidBody->removeCollider(Co);
    }
    CollisionShapes.Reset();
}

void URp3dRigidBody::UpdateMassPropertiesFromColliders()
{
    RigidBody->updateMassPropertiesFromColliders();
}

void URp3dRigidBody::EnableGravity(bool Val)
{
    RigidBody->enableGravity(Val);
}

