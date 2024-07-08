#pragma once 

#include "CoreMinimal.h"
#include "Rp3dCollisionShape.generated.h"

UCLASS()
class RP3DRUNTIME_API URp3dCollisionShape: public UObject
{
	GENERATED_BODY()
public:
	reactphysics3d::CollisionShape* GetRp3dShape(){return Shape.Get(); }

	static URp3dCollisionShape* CreateBoxShape(const FVector& BoxExtents);
	static URp3dCollisionShape* CreateSphereShape(float Radius);
	static URp3dCollisionShape* CreateCapsuleShape(float Radius, float HalfHeight);
	static URp3dCollisionShape* CreateConvexShape(const FKConvexElem& Convex, const FVector& Scale);
	static URp3dCollisionShape* CreateConcaveShape(const TArray<FVector>& Vertices, const TArray<int32>& Indices, const FVector& Scale);

	virtual void BeginDestroy() override;

protected:
	TSharedPtr<reactphysics3d::CollisionShape> Shape;
};


UCLASS()
class RP3DRUNTIME_API URp3dConvexShape: public URp3dCollisionShape
{
	GENERATED_BODY()
public:
	static URp3dConvexShape* Create(const FKConvexElem& Convex, const FVector& Scale);

	virtual void BeginDestroy() override;

private:
	TSharedPtr<reactphysics3d::ConvexMesh> Mesh;

};

UCLASS()
class RP3DRUNTIME_API URp3dConcaveShape: public URp3dCollisionShape
{
	GENERATED_BODY()
public:
	static URp3dConcaveShape* Create(const TArray<FVector>& Vertices, const TArray<int32>& Indices, const FVector& Scale);

	virtual void BeginDestroy() override;
private:
	TSharedPtr<reactphysics3d::TriangleMesh> TriangleMesh;
};