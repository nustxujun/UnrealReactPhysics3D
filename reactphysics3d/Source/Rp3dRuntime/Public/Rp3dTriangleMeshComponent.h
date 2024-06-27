#pragma once 

#include "CoreMinimal.h"
#include "Rp3dShapeComponent.h"

#include "Rp3dTriangleMeshComponent.generated.h"

UCLASS(ClassGroup = "reactphysics3d", meta = (DisplayName = "reactphysics3d Triangle Mesh Collision", BlueprintSpawnableComponent))
class RP3DRUNTIME_API URp3dTriangleMeshComponent : public URp3dShapeComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, export, Category = "reactphysics3d")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, export, Category = "reactphysics3d")
	bool bIsConvexMesh  = false;


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	FPrimitiveSceneProxy* CreateSceneProxy() override;


public:
	void UpdateCollisionShape() override;

private:


};