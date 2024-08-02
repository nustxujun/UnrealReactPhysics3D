#pragma once 

#include "CoreMinimal.h"

class RP3DRUNTIME_API Rp3dCollisionShape
{
public:
	using Ptr = TSharedPtr<Rp3dCollisionShape>;
	reactphysics3d::CollisionShape* GetRp3dShape(){return Shape.Get(); }

	static Ptr CreateBoxShape(const FVector& BoxExtents);
	static Ptr CreateSphereShape(float Radius);
	static Ptr CreateCapsuleShape(float Radius, float HalfHeight);
	static Ptr CreateConvexShape(const FKConvexElem& Convex, const FVector& Scale);
	static Ptr CreateConcaveShape(const TArray<FVector>& Vertices, const TArray<int32>& Indices, const FVector& Scale);

protected:
	TSharedPtr<reactphysics3d::CollisionShape> Shape;
};


class RP3DRUNTIME_API Rp3dConvexShape: public Rp3dCollisionShape
{
public:
	static TSharedPtr<Rp3dConvexShape> Create(const FKConvexElem& Convex, const FVector& Scale);

private:
	TSharedPtr<reactphysics3d::ConvexMesh> Mesh;

};

class RP3DRUNTIME_API Rp3dConcaveShape: public Rp3dCollisionShape
{
public:
	static TSharedPtr<Rp3dConcaveShape> Create(const TArray<FVector>& Vertices, const TArray<int32>& Indices, const FVector& Scale);
private:
	TSharedPtr<reactphysics3d::TriangleMesh> TriangleMesh;
};