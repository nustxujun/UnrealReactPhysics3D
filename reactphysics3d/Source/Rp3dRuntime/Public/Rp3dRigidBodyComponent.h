#pragma once 

#include "CoreMinimal.h"
#include "Rp3dCommon.h"
#include "Rp3dCollisionShape.h"
#include "Rp3dRigidBodyComponent.generated.h"

class URp3dWorld;
enum class EBodyType;

UCLASS(abstract, ClassGroup = reactphysics3d, showcategories = (Mobility))
class RP3DRUNTIME_API URp3dRigidBodyComponent : public UPrimitiveComponent
{
	GENERATED_UCLASS_BODY()
public:
#if WITH_EDITORONLY_DATA	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "reactphysics3d")
	bool bEnableDebugDraw = false;

#endif
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	virtual void BeginPlay()override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnCreateRp3dState();
	virtual void OnDestroyRp3dState();

	void SetBodyType(EBodyType Type);


	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
	void AddVelocity(const FVector& Vel);

	UFUNCTION(BlueprintCallable, Category = "reactphysics3d")
	void SetVelocity(const FVector& Vel);

	void SetAngularLockAxisFactor(const FVector& Factor);
protected:
	UPROPERTY()
	URp3dRigidBody* RigidBody;

	FVector Scale3D;

};