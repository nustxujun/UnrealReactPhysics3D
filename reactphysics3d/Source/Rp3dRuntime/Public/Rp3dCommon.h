#pragma once 

#include "reactphysics3d/decimal.h"

namespace reactphysics3d
{
	class Collider;
	class CollisionShape;
	class ConvexMesh;
	class ConvexMeshShape;
	class ConcaveMeshShape;
	class PhysicsCommon;
	class PhysicsWorld;
	class RigidBody;
	class TriangleMesh;
}

struct FRp3dCollider;

class URp3dCollisionShape;
class URp3dRigidBody;
class URp3dWorld;


#include "reactphysics3d/mathematics/Vector3.h"
#include "reactphysics3d/mathematics/Transform.h"
#include "reactphysics3d/mathematics/Quaternion.h"


using Rp3dTransform = reactphysics3d::Transform;
using Rp3dVector3 = reactphysics3d::Vector3;
using Rp3dQuat = reactphysics3d::Quaternion;
