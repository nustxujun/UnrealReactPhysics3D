#include "Rp3dRigidBody.h"
#include "Rp3dSystem.h"
#include "Rp3dWorld.h"
#include "Rp3dUtils.h"
#include "Rp3dCollisionShape.h"

#include "reactphysics3d/engine/PhysicsWorld.h"
#include "reactphysics3d/engine/PhysicsCommon.h"




void FRp3dCollider::SetBounciness(float Val)
{
    if (!IsValid())
        return;
    Collider->getMaterial().setBounciness(Val);
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

void URp3dRigidBody::RemoveFromWorld()
{
    PhysicsWorld->RemoveRigidBody(this);
    RigidBody.Reset();
    PhysicsWorld = nullptr;
}

void URp3dRigidBody::BeginDestroy()
{
    Super::BeginDestroy();
    PhysicsWorld = nullptr;
    RigidBody.Reset();
}

bool URp3dRigidBody::IsActive()const
{
    return RigidBody->isActive();
}

FRp3dCollider URp3dRigidBody::AddCollisionShape(URp3dCollisionShape* Shape, const Rp3dTransform& Trans)
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

