#include "Rp3dRigidBody.h"
#include "Rp3dSystem.h"
#include "Rp3dWorld.h"
#include "Rp3dUtils.h"
#include "Rp3dCollisionShape.h"

#include "reactphysics3d/engine/PhysicsWorld.h"
#include "reactphysics3d/engine/PhysicsCommon.h"




bool FRp3dCollider::IsValid() const
{
    return RigidBody.IsValid();
}

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

void FRp3dCollider::Initialize(reactphysics3d::Collider* Co, TWeakPtr<reactphysics3d::RigidBody> Rb)
{
    Collider = Co;
    RigidBody = Rb;
}

URp3dRigidBody::URp3dRigidBody(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URp3dRigidBody::Initialize(URp3dWorld* RWorld) 
{
    PhysicsWorld = RWorld;
    auto Rp3dWorld = PhysicsWorld->GetRp3dPhysicsWorld();

    RigidBody = Rp3dWorld->CreateRigidBody();

    RWorld->AddRigidBody(this);
}

void URp3dRigidBody::SetBodyType(EBodyType Type)
{
	RigidBody->SetBodyType(Type);
}

void URp3dRigidBody::SetIsDebugEnabled(bool Enable)
{
    RigidBody->SetIsDebugEnabled(Enable);
}


void URp3dRigidBody::SetTransform(const Rp3dTransform& Trans)
{
    RigidBody->SetTransform(Trans);
}

const Rp3dTransform& URp3dRigidBody::GetTransform()const
{
    return RigidBody->GetTransform();
}

Rp3dVector3 URp3dRigidBody::GetLinearVelocity()const
{
    return RigidBody->GetLinearVelocity();
}

void URp3dRigidBody::SetLinearDamping(reactphysics3d::decimal Damping)
{
    RigidBody->SetLinearDamping(Damping);
}

void URp3dRigidBody::SetLinearVelocity(const Rp3dVector3& Vel)
{
    RigidBody->SetLinearVelocity(Vel);
}

Rp3dVector3 URp3dRigidBody::GetAngularVelocity()const
{
    return RigidBody->GetAngularVelocity();
}
void URp3dRigidBody::SetAngularVelocity(const Rp3dVector3& Vel)
{
    RigidBody->SetAngularVelocity(Vel);
}




void URp3dRigidBody::SetAngularLockAxisFactor(const Rp3dVector3& Factor)
{
	RigidBody->SetAngularLockAxisFactor(Factor);
}


void URp3dRigidBody::ApplyForceAtCenterOfMass(const Rp3dVector3& Force, bool bLocalSpace )
{
    RigidBody->ApplyForceAtCenterOfMass(Force, bLocalSpace);
}

void URp3dRigidBody::ApplyForceAtPoint(const Rp3dVector3& Force, const Rp3dVector3& Point, bool bLocalForce , bool bLocalPoint )
{
    RigidBody->ApplyForceAtPoint(Force, Point, bLocalForce, bLocalPoint);
}

const Rp3dVector3& URp3dRigidBody::GetForce()const
{
    return RigidBody->GetForce();
}

void URp3dRigidBody::ResetForce()
{
    RigidBody->ResetForce();
}

void URp3dRigidBody::RemoveFromWorld()
{
    RigidBody.Reset();
    if (PhysicsWorld)
    {
        PhysicsWorld->RemoveRigidBody(this);
        PhysicsWorld = nullptr;
    }
}

reactphysics3d::decimal URp3dRigidBody::GetMass()const
{
    return RigidBody->GetMass();
}


void URp3dRigidBody::BeginDestroy()
{
    Super::BeginDestroy();
    RemoveFromWorld();
}

bool URp3dRigidBody::IsActive()const
{
    return RigidBody->IsActive();
}

FRp3dCollider URp3dRigidBody::AddCollisionShape(Rp3dCollisionShape::Ptr Shape, const Rp3dTransform& Trans)
{
    return RigidBody->AddCollisionShape(Shape, Trans);
}

void URp3dRigidBody::RemoveAllCollisionShapes()
{
    RigidBody->RemoveAllCollisionShapes();
}

void URp3dRigidBody::UpdateMassPropertiesFromColliders()
{
    RigidBody->UpdateMassPropertiesFromColliders();
}

void URp3dRigidBody::EnableGravity(bool Val)
{
    RigidBody->EnableGravity(Val);
}

FRp3dRigidBody::FRp3dRigidBody(FRp3dPhysicsWorld* World)
{
    RigidBody = TSharedPtr<reactphysics3d::RigidBody>(World->PhysicsWorld->createRigidBody({ UE_TO_RP3D(FTransform::Identity) }),
        [PhysicsWorld = World->PhysicsWorld](auto Body) {
            PhysicsWorld->destroyRigidBody(Body);
        });
}



void FRp3dRigidBody::SetBodyType(EBodyType Type)
{
    using namespace reactphysics3d;
    switch (Type)
    {
    case EBodyType::STATIC: RigidBody->setType(BodyType::STATIC); break;
    case EBodyType::KINEMATIC: RigidBody->setType(BodyType::KINEMATIC); break;
    case EBodyType::DYNAMIC: RigidBody->setType(BodyType::DYNAMIC); break;
    }
}

void FRp3dRigidBody::SetIsDebugEnabled(bool Enable)
{
    RigidBody->setIsDebugEnabled(Enable);
}


void FRp3dRigidBody::SetTransform(const Rp3dTransform& Trans)
{
    RigidBody->setTransform(Trans);
}

const Rp3dTransform& FRp3dRigidBody::GetTransform()const
{
    return RigidBody->getTransform();
}

Rp3dVector3 FRp3dRigidBody::GetLinearVelocity()const
{
    return RigidBody->getLinearVelocity();
}

void FRp3dRigidBody::SetLinearDamping(reactphysics3d::decimal Damping)
{
    RigidBody->setLinearDamping(Damping);
}

void FRp3dRigidBody::SetLinearVelocity(const Rp3dVector3& Vel)
{
    RigidBody->setLinearVelocity(Vel);
}

Rp3dVector3 FRp3dRigidBody::GetAngularVelocity()const
{
    return RigidBody->getAngularVelocity();
}
void FRp3dRigidBody::SetAngularVelocity(const Rp3dVector3& Vel)
{
    RigidBody->setAngularVelocity(Vel);
}




void FRp3dRigidBody::SetAngularLockAxisFactor(const Rp3dVector3& Factor)
{
    RigidBody->setAngularLockAxisFactor(Factor);
}


void FRp3dRigidBody::ApplyForceAtCenterOfMass(const Rp3dVector3& Force, bool bLocalSpace)
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

void FRp3dRigidBody::ApplyForceAtPoint(const Rp3dVector3& Force, const Rp3dVector3& Point, bool bLocalForce, bool bLocalPoint)
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

const Rp3dVector3& FRp3dRigidBody::GetForce()const
{
    return RigidBody->getForce();
}

void FRp3dRigidBody::ResetForce()
{
    RigidBody->resetForce();
}

reactphysics3d::decimal FRp3dRigidBody::GetMass()const
{
    return RigidBody->getMass();
}


bool FRp3dRigidBody::IsActive()const
{
    return RigidBody->isActive();
}

FRp3dCollider FRp3dRigidBody::AddCollisionShape(Rp3dCollisionShape::Ptr Shape, const Rp3dTransform& Trans)
{
    auto Collider = RigidBody->addCollider(Shape->GetRp3dShape(), Trans);
    CollisionShapes.Add(Shape);
    FRp3dCollider RCollider;
    RCollider.Initialize(Collider, RigidBody);
    return RCollider;
}

void FRp3dRigidBody::RemoveAllCollisionShapes()
{
    while (RigidBody && RigidBody->getNbColliders())
    {
        auto Co = RigidBody->getCollider(RigidBody->getNbColliders() - 1);
        RigidBody->removeCollider(Co);
    }
    CollisionShapes.Reset();
}

void FRp3dRigidBody::UpdateMassPropertiesFromColliders()
{
    RigidBody->updateMassPropertiesFromColliders();
}

void FRp3dRigidBody::EnableGravity(bool Val)
{
    RigidBody->enableGravity(Val);
}
