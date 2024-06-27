#pragma once 

#include "CoreMinimal.h"
#include "Rp3dShapeComponent.h"

#include "Rp3dBoxComponent.generated.h"

UCLASS(ClassGroup = "reactphysics3d",   meta = (DisplayName = "reactphysics3d Box Collision", BlueprintSpawnableComponent))
class RP3DRUNTIME_API URp3dBoxComponent : public URp3dShapeComponent
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = "reactphysics3d|Box")
	FVector BoxExtent = {32.0f, 32.0f, 32.0f};

public:

	//~ Begin UPrimitiveComponent Interface.
	virtual bool IsZeroExtent() const override;
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual struct FCollisionShape GetCollisionShape(float Inflation = 0.0f) const override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End USceneComponent Interface



	void UpdateCollisionShape() override;
private:
};