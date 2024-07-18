#pragma once 

#include "CoreMinimal.h"
#include "Rp3dCommon.h"

#include "Rp3dRigidBody.generated.h"
enum class EBodyType
{
	STATIC,
	KINEMATIC,
	DYNAMIC,
};


USTRUCT()
struct RP3DRUNTIME_API FRp3dCollider
{
    GENERATED_BODY()

    bool IsValid(){return RigidBody.IsValid();};
    void SetBounciness(float Val);

    void Initialize(reactphysics3d::Collider* Co, TWeakObjectPtr<URp3dRigidBody> Rb)
    {
        Collider = Co;
        RigidBody = Rb;
    }
private:
    reactphysics3d::Collider* Collider;
    TWeakObjectPtr<URp3dRigidBody> RigidBody;
};

UCLASS(Blueprintable, BlueprintType)
class RP3DRUNTIME_API URp3dRigidBody: public UObject
{
    GENERATED_UCLASS_BODY()
public:
    void Initialize(URp3dWorld* RWorld);
    URp3dWorld* GetPhysicsWorld(){return PhysicsWorld;}
    

	void SetBodyType(EBodyType Type);
    void SetIsDebugEnabled(bool Enable);

    void SetTransform(const Rp3dTransform& Trans);
    const Rp3dTransform& GetTransform()const;

    void SetLinearVelocity(const Rp3dVector3& Vel);
    Rp3dVector3 GetLinearVelocity()const;

    Rp3dVector3 GetAngularVelocity()const;
    void SetAngularVelocity(const Rp3dVector3& Vel);

	void SetAngularLockAxisFactor(const Rp3dVector3& Factor);

    FRp3dCollider AddCollisionShape(URp3dCollisionShape* Shape, const Rp3dTransform& Trans = Rp3dTransform::identity());
    void RemoveAllCollisionShapes();
	void UpdateMassPropertiesFromColliders();
    void EnableGravity(bool Val);

    bool IsActive()const;

    void RemoveFromWorld();

    virtual void BeginDestroy() override;
private:
	TSharedPtr<reactphysics3d::RigidBody> RigidBody;
    UPROPERTY()
    URp3dWorld* PhysicsWorld = nullptr;
    UPROPERTY()
    TArray<URp3dCollisionShape*> CollisionShapes;
};